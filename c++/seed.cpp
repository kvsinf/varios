
#include <iostream>
#include <string>
#include <curl/curl.h>

using namespace std;

size_t recibir(char* datos, size_t tam, size_t n, void* destino) {
    ((string*)destino)->append(datos, tam * n);
    return tam * n;
}

string extraer(const string& xml, const string& etiqueta) {
    string inicio = "<" + etiqueta + ">";
    string fin = "</" + etiqueta + ">";

    size_t a = xml.find(inicio);
    if (a == string::npos) return "";

    a += inicio.size();

    size_t b = xml.find(fin, a);
    if (b == string::npos) return "";

    return xml.substr(a, b - a);
}

int main() {
    curl_global_init(CURL_GLOBAL_DEFAULT);

    CURL* c = curl_easy_init();
    if (!c) {
        curl_global_cleanup();
        return 1;
    }

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

    long http = 0;
    curl_easy_getinfo(c, CURLINFO_RESPONSE_CODE, &http);

    curl_slist_free_all(h);
    curl_easy_cleanup(c);
    curl_global_cleanup();

    if (resultado != CURLE_OK || http != 200) {
        cerr << "Error consultando SII. HTTP: " << http << endl;
        return 1;
    }

    // La respuesta SOAP contiene un XML interno escapado.
    // Extraemos SEMILLA y ESTADO de ese contenido.
    string semilla = extraer(respuesta, "SEMILLA");
    string estado = extraer(respuesta, "ESTADO");

    if (semilla.empty()) {
        // SOAP puede devolver el XML interno con entidades HTML.
        string normalizado = respuesta;
        size_t pos = 0;
        while ((pos = normalizado.find("&lt;", pos)) != string::npos)
            normalizado.replace(pos, 4, "<");

        pos = 0;
        while ((pos = normalizado.find("&gt;", pos)) != string::npos)
            normalizado.replace(pos, 4, ">");

        semilla = extraer(normalizado, "SEMILLA");
        estado = extraer(normalizado, "ESTADO");
    }

    if (estado == "00" && !semilla.empty()) {
        cout << "SEMILLA: " << semilla << endl;
    } else {
        cerr << "No se pudo obtener la semilla. Estado: "
             << estado << endl;
        return 1;
    }

    return 0;
}
