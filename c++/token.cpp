#include <curl/curl.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/c14n.h>
#include <openssl/pkcs12.h>
#include <openssl/evp.h>
#include <openssl/x509.h>
#include <openssl/provider.h>
#include <openssl/err.h>
#include <fstream>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
#include <sys/stat.h>

using namespace std;
// Mantener los proveedores vivos durante toda la lectura del PFX y la firma.
struct OpenSSLProviders {
    OSSL_PROVIDER* standard = nullptr;
    OSSL_PROVIDER* legacy = nullptr;
    OpenSSLProviders() {
        standard = OSSL_PROVIDER_load(nullptr, "default");
        if (!standard) throw runtime_error("No se pudo cargar OpenSSL default");
        legacy = OSSL_PROVIDER_load(nullptr, "legacy");
        // Legacy es opcional: los certificados modernos siguen funcionando.
        if (!legacy) ERR_clear_error();
    }
    ~OpenSSLProviders() {
        if (legacy) OSSL_PROVIDER_unload(legacy);
        if (standard) OSSL_PROVIDER_unload(standard);
    }
};
static void check(bool ok, const string& msg) { if (!ok) throw runtime_error(msg); }
static size_t receive(char* p, size_t s, size_t n, void* u) {
    static_cast<string*>(u)->append(p, s*n); return s*n;
}
static string xpath(xmlDocPtr doc, const char* expr) {
    xmlXPathContextPtr ctx=xmlXPathNewContext(doc);
    check(ctx!=nullptr,"Error creando XPath");
    xmlXPathObjectPtr obj=xmlXPathEvalExpression(BAD_CAST expr,ctx);
    string out;
    if(obj && obj->type==XPATH_NODESET && obj->nodesetval && obj->nodesetval->nodeNr) {
        xmlChar* v=xmlNodeGetContent(obj->nodesetval->nodeTab[0]);
        if(v){out=(char*)v;xmlFree(v);}
    }
    if(obj) xmlXPathFreeObject(obj);
    xmlXPathFreeContext(ctx);
    return out;
}
static string getSeed() {
    const string body="<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
        "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\" "
        "xmlns:ser=\"http://DefaultNamespace\"><soapenv:Header/>"
        "<soapenv:Body><ser:getSeed/></soapenv:Body></soapenv:Envelope>";
    CURL* c=curl_easy_init(); check(c!=nullptr,"curl_easy_init fallo");
    curl_slist* headers=nullptr;
    headers=curl_slist_append(headers,"Content-Type: text/xml; charset=utf-8");
    headers=curl_slist_append(headers,"SOAPAction: \"\"");
    string response;
    curl_easy_setopt(c,CURLOPT_URL,"https://palena.sii.cl/DTEWS/CrSeed.jws");
    curl_easy_setopt(c,CURLOPT_HTTPHEADER,headers);
    curl_easy_setopt(c,CURLOPT_POSTFIELDS,body.c_str());
    curl_easy_setopt(c,CURLOPT_POSTFIELDSIZE,(long)body.size());
    curl_easy_setopt(c,CURLOPT_WRITEFUNCTION,receive);
    curl_easy_setopt(c,CURLOPT_WRITEDATA,&response);
    curl_easy_setopt(c,CURLOPT_TIMEOUT,30L);
    CURLcode rc=curl_easy_perform(c); long http=0;
    curl_easy_getinfo(c,CURLINFO_RESPONSE_CODE,&http);
    curl_slist_free_all(headers);curl_easy_cleanup(c);
    check(rc==CURLE_OK,string("Error HTTP: ")+curl_easy_strerror(rc));
    check(http==200,"SII respondio HTTP "+to_string(http));
    xmlDocPtr soap=xmlReadMemory(response.data(),(int)response.size(),"soap.xml",nullptr,XML_PARSE_NONET);
    check(soap!=nullptr,"Respuesta SOAP invalida");
    string inner=xpath(soap,"//*[local-name()='getSeedReturn']");xmlFreeDoc(soap);
    check(!inner.empty(),"No existe getSeedReturn");
    xmlDocPtr doc=xmlReadMemory(inner.data(),(int)inner.size(),"seed.xml",nullptr,XML_PARSE_NONET);
    check(doc!=nullptr,"XML interno de semilla invalido");
    string status=xpath(doc,"//*[local-name()='ESTADO']");
    string seed=xpath(doc,"//*[local-name()='SEMILLA']");xmlFreeDoc(doc);
    check(status=="00" && !seed.empty(),"SII no entrego semilla valida. ESTADO="+status);
    return seed;
}
static string b64(const unsigned char* data, size_t n) {
    string out(4*((n+2)/3),'\0');
    int k=EVP_EncodeBlock((unsigned char*)&out[0],data,(int)n);
    check(k>=0,"Error Base64");out.resize(k);return out;
}
static string canonical(xmlDocPtr doc, xmlNodePtr node) {
    // Canonicalizacion inclusiva C14N 1.0 sobre el nodo y sus descendientes.
    // Para documentos separados, canonicalizamos el documento completo.
    (void)node;
    xmlChar* result=nullptr;
    int n=xmlC14NDocDumpMemory(doc,nullptr,XML_C14N_1_0,nullptr,0,&result);
    check(n>=0 && result!=nullptr,"Fallo C14N");
    string out((char*)result,n);xmlFree(result);return out;
}
static xmlNodePtr child(xmlNodePtr parent, xmlNsPtr ns, const char* name, const string& value="") {
    return xmlNewChild(parent,ns,BAD_CAST name,value.empty()?nullptr:BAD_CAST value.c_str());
}
static string readPassword(const string& path) {
    ifstream f(path,ios::binary);check((bool)f,"No se pudo leer "+path);
    string p((istreambuf_iterator<char>(f)),istreambuf_iterator<char>());
    while(!p.empty() && (p.back()=='\n'||p.back()=='\r'))p.pop_back();
    check(!p.empty(),"Archivo de clave vacio");return p;
}
static string signSeed(const string& seed,const string& pfxPath,const string& passPath) {
    string pass=readPassword(passPath);
    FILE* fp=fopen(pfxPath.c_str(),"rb");check(fp!=nullptr,"No se pudo abrir "+pfxPath);
    PKCS12* pkcs=d2i_PKCS12_fp(fp,nullptr);fclose(fp);
    check(pkcs!=nullptr,"PFX invalido o formato no compatible con OpenSSL");
    EVP_PKEY* key=nullptr;X509* cert=nullptr;STACK_OF(X509)* chain=nullptr;
    int parsed=PKCS12_parse(pkcs,pass.c_str(),&key,&cert,&chain);
    PKCS12_free(pkcs);
    if (parsed != 1 || !key || !cert) {
        unsigned long e = ERR_peek_last_error();
        char detalle[256] = {0};
        if (e) ERR_error_string_n(e, detalle, sizeof(detalle));
        if (key) EVP_PKEY_free(key);
        if (cert) X509_free(cert);
        if (chain) sk_X509_pop_free(chain, X509_free);
        throw runtime_error(string("No se pudo leer PFX (clave, formato o cifrado): ") + detalle);
    }
    if(chain)sk_X509_pop_free(chain,X509_free);
    check(EVP_PKEY_base_id(key)==EVP_PKEY_RSA,"Este flujo SII requiere clave RSA");
    check(X509_cmp_current_time(X509_get0_notBefore(cert))<=0 &&
          X509_cmp_current_time(X509_get0_notAfter(cert))>=0,"Certificado fuera de vigencia");

    xmlDocPtr doc=xmlNewDoc(BAD_CAST "1.0");check(doc!=nullptr,"Error creando XML");
    xmlNodePtr root=xmlNewNode(nullptr,BAD_CAST "getToken");xmlDocSetRootElement(doc,root);
    xmlNodePtr item=child(root,nullptr,"item");child(item,nullptr,"Semilla",seed);
    // Digest sobre getToken SIN Signature: transform enveloped-signature.
    string signedDocument=canonical(doc,root);
    unsigned char digest[EVP_MAX_MD_SIZE];unsigned int digestLen=0;
    check(EVP_Digest(signedDocument.data(),signedDocument.size(),digest,&digestLen,EVP_sha1(),nullptr)==1,"Error SHA1 digest");

    const char* ds="http://www.w3.org/2000/09/xmldsig#";
    xmlNodePtr signature=xmlNewNode(nullptr,BAD_CAST "Signature");
    xmlNsPtr ns=xmlNewNs(signature,BAD_CAST ds,nullptr);xmlSetNs(signature,ns);
    xmlNodePtr si=child(signature,ns,"SignedInfo");
    xmlNodePtr canon=child(si,ns,"CanonicalizationMethod");
    xmlNewProp(canon,BAD_CAST "Algorithm",BAD_CAST "http://www.w3.org/TR/2001/REC-xml-c14n-20010315");
    xmlNodePtr method=child(si,ns,"SignatureMethod");
    xmlNewProp(method,BAD_CAST "Algorithm",BAD_CAST "http://www.w3.org/2000/09/xmldsig#rsa-sha1");
    xmlNodePtr ref=child(si,ns,"Reference");xmlNewProp(ref,BAD_CAST "URI",BAD_CAST "");
    xmlNodePtr transforms=child(ref,ns,"Transforms");
    xmlNodePtr transform=child(transforms,ns,"Transform");
    xmlNewProp(transform,BAD_CAST "Algorithm",BAD_CAST "http://www.w3.org/2000/09/xmldsig#enveloped-signature");
    xmlNodePtr dm=child(ref,ns,"DigestMethod");
    xmlNewProp(dm,BAD_CAST "Algorithm",BAD_CAST "http://www.w3.org/2000/09/xmldsig#sha1");
    child(ref,ns,"DigestValue",b64(digest,digestLen));

    // SignedInfo debe canonizarse con el namespace dsig en contexto.
    xmlDocPtr sidoc=xmlNewDoc(BAD_CAST "1.0");
    xmlNodePtr siCopy=xmlDocCopyNode(si,sidoc,1);xmlDocSetRootElement(sidoc,siCopy);
    string canonicalSI=canonical(sidoc,siCopy);xmlFreeDoc(sidoc);
    EVP_MD_CTX* ctx=EVP_MD_CTX_new();check(ctx!=nullptr,"Error EVP_MD_CTX_new");
    size_t signatureLen=0;
    check(EVP_DigestSignInit(ctx,nullptr,EVP_sha1(),nullptr,key)==1 &&
          EVP_DigestSignUpdate(ctx,canonicalSI.data(),canonicalSI.size())==1 &&
          EVP_DigestSignFinal(ctx,nullptr,&signatureLen)==1,"Error preparando firma");
    vector<unsigned char> signatureBytes(signatureLen);
    check(EVP_DigestSignFinal(ctx,signatureBytes.data(),&signatureLen)==1,"Error firmando SignedInfo");
    EVP_MD_CTX_free(ctx);
    child(signature,ns,"SignatureValue",b64(signatureBytes.data(),signatureLen));
    xmlNodePtr ki=child(signature,ns,"KeyInfo");
    xmlNodePtr xdata=child(ki,ns,"X509Data");
    int certLen=i2d_X509(cert,nullptr);check(certLen>0,"Error exportando certificado");
    vector<unsigned char> der(certLen);unsigned char* derp=der.data();i2d_X509(cert,&derp);
    child(xdata,ns,"X509Certificate",b64(der.data(),der.size()));
    xmlAddChild(root,signature);
    EVP_PKEY_free(key);X509_free(cert);

    xmlChar* xmlOut=nullptr;int outLen=0;
    xmlDocDumpFormatMemoryEnc(doc,&xmlOut,&outLen,"UTF-8",0);
    check(xmlOut!=nullptr && outLen>0,"No se pudo serializar XML");
    string signedXml((char*)xmlOut, outLen);
    xmlFree(xmlOut);
    xmlFreeDoc(doc);
    return signedXml;
}

// PASO 3: enviar XML firmado desde memoria y obtener token del SII.
static string getToken(const string& signedXml) {
    check(!signedXml.empty(), "XML firmado vacio");
    xmlChar* escaped = xmlEncodeSpecialChars(nullptr, BAD_CAST signedXml.c_str());
    check(escaped != nullptr, "No se pudo escapar XML firmado");
    string safeXml((char*)escaped);
    xmlFree(escaped);
    string body = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
        "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\" "
        "xmlns:ser=\"http://DefaultNamespace\">"
        "<soapenv:Body><ser:getToken><ser:pszXml>" + safeXml +
        "</ser:pszXml></ser:getToken></soapenv:Body></soapenv:Envelope>";
    CURL* c=curl_easy_init();check(c!=nullptr,"curl_easy_init fallo");
    curl_slist* headers=nullptr;
    headers=curl_slist_append(headers,"Content-Type: text/xml; charset=utf-8");
    headers=curl_slist_append(headers,"SOAPAction: \"\"");
    string response;
    curl_easy_setopt(c,CURLOPT_URL,"https://palena.sii.cl/DTEWS/GetTokenFromSeed.jws");
    curl_easy_setopt(c,CURLOPT_HTTPHEADER,headers);
    curl_easy_setopt(c,CURLOPT_POSTFIELDS,body.c_str());
    curl_easy_setopt(c,CURLOPT_POSTFIELDSIZE,(long)body.size());
    curl_easy_setopt(c,CURLOPT_WRITEFUNCTION,receive);
    curl_easy_setopt(c,CURLOPT_WRITEDATA,&response);
    curl_easy_setopt(c,CURLOPT_TIMEOUT,30L);
    CURLcode rc=curl_easy_perform(c);long http=0;
    curl_easy_getinfo(c,CURLINFO_RESPONSE_CODE,&http);
    curl_slist_free_all(headers);curl_easy_cleanup(c);
    check(rc==CURLE_OK,string("Error solicitando token: ")+curl_easy_strerror(rc));
    check(http==200,"GetTokenFromSeed respondio HTTP "+to_string(http));
    xmlDocPtr soap=xmlReadMemory(response.data(),(int)response.size(),"token_soap.xml",nullptr,XML_PARSE_NONET);
    check(soap!=nullptr,"Respuesta SOAP de token invalida");
    string inner=xpath(soap,"//*[local-name()='getTokenReturn']");
    string fault=xpath(soap,"//*[local-name()='faultstring']");
    xmlFreeDoc(soap);
    check(!inner.empty(),"No existe getTokenReturn. "+fault);
    xmlDocPtr doc=xmlReadMemory(inner.data(),(int)inner.size(),"token.xml",nullptr,XML_PARSE_NONET);
    check(doc!=nullptr,"XML interno de token invalido");
    string status=xpath(doc,"//*[local-name()='RESP_HDR']/*[local-name()='ESTADO']");
    string token=xpath(doc,"//*[local-name()='TOKEN']");
    string glosa=xpath(doc,"//*[local-name()='GLOSA']");
    xmlFreeDoc(doc);
    check(status=="00","SII rechazo solicitud de token. ESTADO="+status+" "+glosa);
    check(!token.empty(),"SII respondio sin TOKEN");
    return token;
}

int main(int argc,char** argv) {
    string pfx=argc>1?argv[1]:"certificado.pfx";
    string password=argc>2?argv[2]:"clave_pfx.txt";
    if(curl_global_init(CURL_GLOBAL_DEFAULT)!=CURLE_OK){cerr<<"Error inicializando curl\n";return 1;}
    try {
        OpenSSLProviders providers;
        cout<<"1. Solicitando semilla al SII...\n";
        string seed=getSeed();cout<<"   Semilla obtenida correctamente.\n";
        cout<<"2. Firmando XML con "<<pfx<<"...\n";
        string signedXml=signSeed(seed,pfx,password);
        cout<<"   XML firmado en memoria ("<<signedXml.size()<<" bytes).\n";
        cout<<"3. Solicitando token al SII...\n";
        string token=getToken(signedXml);
        cout<<"   Token SII: "<<token<<"\n";
        curl_global_cleanup();return 0;
    } catch(const exception& e) {
        cerr<<"ERROR: "<<e.what()<<"\n";
        curl_global_cleanup();return 1;
    }
}
