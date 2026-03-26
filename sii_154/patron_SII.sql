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
