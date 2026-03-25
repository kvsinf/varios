![version](https://img.shields.io/badge/version-3.0-blue.svg)  ![framework](https://img.shields.io/badge/.NET_Framework-3.5-purple.svg?style=flat-square)

📋 Guía rápida de instalación

1. Doble clic en Setup1.msi.
2. Presionar Next en la pantalla inicial.
3. Aceptar la carpeta por defecto: C:\Program Files (x86)\Default Company Name\Setup1\
4. Seleccionar la opción Everyone.
5. Presionar Next para confirmar.
6. Esperar a que finalice la instalación.
7. Presionar Close para cerrar el asistente.

📋 Guía rápida: Mostrar servicio SeriesBCF35

1. Abrir consola de servicios
   * Presiona Win + R.
   * Escribe services.msc y pulsa Enter.
2. Buscar el servicio
   * En la ventana de Servicios, desplázate por la lista.
   * Localiza SeriesBCF35
3. Ver detalles del servicio
   * Haz doble clic sobre SeriesBCF35.
4. Controlar el servicio
   * Desde la misma ventana o con clic derecho:<br>
         * Iniciar → activa el servicio.<br>
         * Detener → lo desactiva.<br>


📘 Archivo de configuración cfg.txt<br><br>
Este archivo define los parámetros esenciales para la ejecución del servicio SeriesBCF35. Cada línea corresponde a una configuración específica que el sistema interpreta en orden.

📌 Detalle de líneas<br>
Línea 1 → Conexión no encriptada<br>
Línea 2 → Fecha de inicio (formato dd-MM-YYYY)<br>
Línea 3 → Usuario Banco Central<br>
Línea 4 → Contraseña Banco Central<br>
Línea 5 → Hora de inicio de ejecución (formato HH:mm)
Línea 6 → Tiempo en minutos para reintento, rango de 1 a 60

Cambios implementados
1- Se actualizó la información de cultura a es-CL (Español - Chile)
2- Correción en la tabla SERIESFERIADOS para que no se repitan los días feriados
3- Reintento automático en caso de error de conexión, vuelve a intentar la descarga con una frecuencia configurable en minutos, dentro de un rango de 1 a 60 minutos.

