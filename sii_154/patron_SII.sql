SELECT *
FROM patron_SII
WHERE id_patronSII IN ('Patente','PatenteC','RUTTrans','RUTChofer','NombreChof','DirDest','CmnaDest','CiudadDest')
ORDER BY CASE id_patronSII
           WHEN 'Patente'   THEN 1
           WHEN 'PatenteC'  THEN 2
           WHEN 'RUTTrans'  THEN 3
           WHEN 'RUTChofer' THEN 4
           WHEN 'NombreChof' THEN 5
           WHEN 'DirDest' THEN 6
           WHEN 'CmnaDest' THEN 7
           WHEN 'CiudadDest' THEN 8
         END;


DELETE
FROM patron_SII
WHERE id_patronSII IN ('Patente','PatenteC','RUTTrans','RUTChofer',
                       'NombreChof','DirDest','CmnaDest','CiudadDest');


string(descendant::*[local-name()='Patente'])
string(descendant::*[local-name()='PatenteCarro'])
string(descendant::*[local-name()='RUTTrans'])
string(descendant::*[local-name()='RUTChofer'])
string(descendant::*[local-name()='NombreChofer'])

ERROR ----> Documento presenta errores: 
  
Reporte de validaciones del DTE 52-1127
--------------------------------------------------------------------------
 
Falta elemento obligatorio: Transporte Ciudad Destino(DTE/Documento/Encabezado/Transporte/CiudadDest)
Falta elemento obligatorio: Transporte Cmna Destino(DTE/Documento/Encabezado/Transporte/CmnaDest)
Falta elemento obligatorio: Transporte Dir. Destino(DTE/Documento/Encabezado/Transporte/DirDest)
Falta elemento obligatorio: Transporte Nombre Chofer(DTE/Documento/Encabezado/Transporte/NombreChofer)
Falta elemento obligatorio: Transporte Patente(DTE/Documento/Encabezado/Transporte/Patente)
Falta elemento obligatorio: Transporte Patente Carro(DTE/Documento/Encabezado/Transporte/PatenteCarro)
Falta elemento obligatorio: Transporte Rut Chofer(DTE/Documento/Encabezado/Transporte/RUTChofer)
Falta elemento obligatorio: Transporte Rut Transp.(DTE/Documento/Encabezado/Transporte/RUTTrans)

error:
Error de Schema: El elemento 'Transporte' en espacio de nombres 'http://www.sii.cl/SiiDte' tiene un elemento secundario 'PatenteCarro' en espacio de nombres 'http://www.sii.cl/SiiDte' no válido. Lista esperada de elementos posibles: 'RUTTrans, Chofer, DirDest, CmnaDest, CiudadDest, Aduana' en espacio de nombres 'http://www.sii.cl/SiiDte'.


