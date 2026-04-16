SELECT 
    elem, 
    valor
FROM 
    configuracion
WHERE 
    elem IN (
        'Server_IMAP',
        'portIMAP',
        'user_IMAP',
        'password_IMAP',
        'POP3/IMAP',
        'tenantid',
        'clientid',
        'scopeimap',
        'carpeta_IMAP'
    );


SELECT elem, valor 
FROM configuracion 
WHERE elem IN ('Server_IMAP', 'portIMAP','user_IMAP','password_IMAP','POP3/IMAP','tenantid','clientid','scopeimap','carpeta_IMAP');
