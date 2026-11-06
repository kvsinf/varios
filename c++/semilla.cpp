
#include <iostream>
#include <string>
#include <curl/curl.h>

using namespace std;

size_t recibir(char* datos, size_t tam, size_t n, void* destino) {
    ((string*)destino)->append(datos, tam * n);
    return tam * n;
}

int main() {
    curl_global_init(CURL_GLOBAL_DEFAULT);

    CURL* c = curl_easy_init();
    if (!c) return 1;

    string respuesta;

    string xml =
        "<soapenv:Envelope "
        "xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\" "
        "xmlns:ser=\"http://DefaultNamespace\">"
        "<soapenv:Body><ser:getSeed/></soapenv:Body>"
        "</soapenv:Envelope>";

    curl_slist* h = nullptr;
    h = curl_slist_append(h, "Content-Type: text/xml; charset=utf-8");
    h = curl_slist_append(h, "SOAPAction: \"\"");

    curl_easy_setopt(c, CURLOPT_URL,
        "https://palena.sii.cl/DTEWS/CrSeed.jws");

    curl_easy_setopt(c, CURLOPT_HTTPHEADER, h);
    curl_easy_setopt(c, CURLOPT_POSTFIELDS, xml.c_str());
    curl_easy_setopt(c, CURLOPT_WRITEFUNCTION, recibir);
    curl_easy_setopt(c, CURLOPT_WRITEDATA, &respuesta);
    curl_easy_setopt(c, CURLOPT_TIMEOUT, 30L);

    CURLcode resultado = curl_easy_perform(c);

    if (resultado == CURLE_OK)
        cout << respuesta << endl;
    else
        cerr << curl_easy_strerror(resultado) << endl;

    curl_slist_free_all(h);
    curl_easy_cleanup(c);
    curl_global_cleanup();

    return resultado == CURLE_OK ? 0 : 1;
}
