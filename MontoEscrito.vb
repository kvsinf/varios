Public Class MontoEscrito

    ' Autor: KVS
    ' Fecha: 20-11-2025
    ' Rango 0 a 999999999

    Public Function GetMontoEscrito(ByVal n As Decimal) As String
        If n < 10 Then
            Return Me.GetUnidad(n)
        ElseIf n < 100 Then
            Return Me.GetDecena(n)
        ElseIf n < 1000 Then
            Return Me.GetCentena(n)
        ElseIf n < 10000 Then
            Return Me.GetUnidadMil(n)
        ElseIf n < 100000 Then
            Return Me.GetDecenaMil(n)
        ElseIf n < 1000000 Then
            Return Me.GetCentenaMil(n)
        ElseIf n < 10000000 Then
            Return Me.GetUnidadMillon(n)
        ElseIf n < 100000000 Then
            Return Me.GetDecenaMillon(n)
        ElseIf n < 1000000000 Then
            Return Me.GetCentenaMillon(n)
        Else
            Return ""
        End If
    End Function

    Private Function GetUnidad(ByVal n As Decimal) As String
        Select Case n
            Case 0 : Return "CERO"
            Case 1 : Return "UN"
            Case 2 : Return "DOS"
            Case 3 : Return "TRES"
            Case 4 : Return "CUATRO"
            Case 5 : Return "CINCO"
            Case 6 : Return "SEIS"
            Case 7 : Return "SIETE"
            Case 8 : Return "OCHO"
            Case 9 : Return "NUEVE"
            Case Else : Return ""
        End Select
    End Function

    Private Function GetDecena(ByVal n As Decimal) As String
        ' Ejemplo: 87
        Select Case n
            Case 10 : Return "DIEZ"
            Case 11 : Return "ONCE"
            Case 12 : Return "DOCE"
            Case 13 : Return "TRECE"
            Case 14 : Return "CATORCE"
            Case 15 : Return "QUINCE"
            Case 16 To 19 : Return "DIECI" & Me.GetUnidad(n Mod 10)
            Case 20 : Return "VEINTE"
            Case 21 To 29
                Return "VEINTI" & Me.GetUnidad(n Mod 10)
            Case 30 : Return "TREINTA"
            Case 31 To 39 : Return "TREINTA Y " & Me.GetUnidad(n Mod 10)
            Case 40 : Return "CUARENTA"
            Case 41 To 49 : Return "CUARENTA Y " & Me.GetUnidad(n Mod 10)
            Case 50 : Return "CINCUENTA"
            Case 51 To 59 : Return "CINCUENTA Y " & Me.GetUnidad(n Mod 10)
            Case 60 : Return "SESENTA"
            Case 61 To 69 : Return "SESENTA Y " & Me.GetUnidad(n Mod 10)
            Case 70 : Return "SETENTA"
            Case 71 To 79 : Return "SETENTA Y " & Me.GetUnidad(n Mod 10)
            Case 80 : Return "OCHENTA"
            Case 81 To 89 : Return "OCHENTA Y " & Me.GetUnidad(n Mod 10)
            Case 90 : Return "NOVENTA"
            Case 91 To 99 : Return "NOVENTA Y " & Me.GetUnidad(n Mod 10)
            Case Else : Return ""
        End Select
    End Function

    Private Function GetCentena(ByVal n As Decimal) As String
        ' Ejemplo: 101
        Dim centena As Decimal = n \ 100
        Dim resto As Decimal = n Mod 100
        Dim monto As String = ""
        Select Case centena
            Case 1
                If centena = 1 AndAlso resto = 0 Then
                    monto = " CIEN "
                Else
                    monto = " CIENTO "
                End If
            Case 2
                monto = " DOSCIENTOS "
            Case 3
                monto = " TRESCIENTOS "
            Case 4
                monto = " CUATROCIENTOS "
            Case 5
                monto = " QUINIENTOS "
            Case 6
                monto = " SEISCIENTOS "
            Case 7
                monto = " SETECIENTOS "
            Case 8
                monto = " OCHOCIENTOS "
            Case 9
                monto = " NOVECIENTOS "
        End Select
        Dim contarDigitos As Integer = 0
        If resto > 0 Then
            contarDigitos = Me.ContarDigitos(resto)
            Select Case contarDigitos
                Case 2
                    Dim decena As Decimal = n Mod Math.Pow(10, contarDigitos)
                    monto += Me.GetDecena(decena)
                Case 1
                    Dim unidad As Decimal = n Mod Math.Pow(10, contarDigitos)
                    monto += Me.GetUnidad(unidad)
            End Select
        End If
        Return monto
    End Function

    Private Function GetUnidadMil(ByVal n As Decimal) As String
        ' Ejemplo: 8.456
        Dim unidadMil As Decimal = n \ 1000
        Dim resto As Decimal = n Mod 1000
        Dim monto As String = ""
        Select Case unidadMil
            Case 1
                monto = " MIL "
            Case 2 To 9
                monto = Me.GetUnidad(unidadMil) & " MIL "
        End Select
        Dim contarDigitos As Integer = 0
        If resto > 0 Then
            contarDigitos = Me.ContarDigitos(resto)
            Select Case contarDigitos
                Case 3
                    Dim centena As Decimal = n Mod Math.Pow(10, contarDigitos)
                    monto += Me.GetCentena(centena)
                Case 2
                    Dim decena As Decimal = n Mod Math.Pow(10, contarDigitos)
                    monto += Me.GetDecena(decena)
                Case 1
                    Dim unidad As Decimal = n Mod Math.Pow(10, contarDigitos)
                    monto += Me.GetUnidad(unidad)
            End Select
        End If
        Return monto
    End Function

    Private Function GetDecenaMil(ByVal n As Decimal) As String
        ' Ejemplo: 17.987
        Dim decenaMil As Decimal = n \ 1000
        Dim resto As Decimal = n Mod 1000
        Dim monto As String = ""
        monto = Me.GetDecena(decenaMil) & " MIL "
        Dim contarDigitos As Integer = 0
        If resto > 0 Then
            contarDigitos = Me.ContarDigitos(resto)
            Select Case contarDigitos
                Case 3
                    Dim centena As Decimal = n Mod Math.Pow(10, contarDigitos)
                    monto += Me.GetCentena(centena)
                Case 2
                    Dim decena As Decimal = n Mod Math.Pow(10, contarDigitos)
                    monto += Me.GetDecena(decena)
                Case 1
                    Dim unidad As Decimal = n Mod Math.Pow(10, contarDigitos)
                    monto += Me.GetUnidad(unidad)
            End Select
        End If
        Return monto
    End Function

    Private Function GetCentenaMil(ByVal n As Decimal) As String
        ' Ejemplo: 105.364   100 --> 999
        Dim centenaMil As Decimal = n \ 1000
        Dim resto As Decimal = n Mod 1000
        Dim monto As String = ""
        monto = Me.GetCentena(centenaMil) & " MIL "
        Dim contarDigitos As Integer = 0
        If resto > 0 Then
            contarDigitos = Me.ContarDigitos(resto)
            Select Case contarDigitos
                Case 3
                    Dim centena As Decimal = n Mod Math.Pow(10, contarDigitos)
                    monto += Me.GetCentena(centena)
                Case 2
                    Dim decena As Decimal = n Mod Math.Pow(10, contarDigitos)
                    monto += Me.GetDecena(decena)
                Case 1
                    Dim unidad As Decimal = n Mod Math.Pow(10, contarDigitos)
                    monto += Me.GetUnidad(unidad)
            End Select
        End If
        Return monto
    End Function

    Private Function GetUnidadMillon(ByVal n As Decimal) As String
        ' Ejemplo: 8.210.741
        Dim unidadMillon As Decimal = n \ 1000000
        Dim resto As Decimal = n Mod 1000000
        Dim monto As String = ""
        Select Case unidadMillon
            Case 1
                monto = Me.GetUnidad(unidadMillon) & " MILLON "
            Case 2 To 9
                monto = Me.GetUnidad(unidadMillon) & " MILLONES "
        End Select
        Dim contarDigitos As Integer = 0
        If resto > 0 Then
            contarDigitos = Me.ContarDigitos(resto)
            Select Case contarDigitos
                Case 6
                    Dim centenaMil As Decimal = n Mod Math.Pow(10, contarDigitos)
                    monto += Me.GetCentenaMil(centenaMil)
                Case 5
                    Dim decenaMil As Decimal = n Mod Math.Pow(10, contarDigitos)
                    monto += Me.GetDecenaMil(decenaMil)
                Case 4
                    Dim unidadMil As Decimal = n Mod Math.Pow(10, contarDigitos)
                    monto += Me.GetUnidadMil(unidadMil)
                Case 3
                    Dim centena As Decimal = n Mod Math.Pow(10, contarDigitos)
                    monto += Me.GetCentena(centena)
                Case 2
                    Dim decena As Decimal = n Mod Math.Pow(10, contarDigitos)
                    monto += Me.GetDecena(decena)
                Case 1
                    Dim unidad As Decimal = n Mod Math.Pow(10, contarDigitos)
                    monto += Me.GetUnidad(unidad)
            End Select
        End If
        Return monto
    End Function

    Private Function GetDecenaMillon(ByVal n As Decimal) As String
        ' Ejemplo 16.567.123
        ' 50.001.345
        Dim decenaMillon As Decimal = n \ 1000000
        Dim resto As Decimal = n Mod 1000000
        Dim monto As String = ""
        monto = Me.GetDecena(decenaMillon) & " MILLONES "
        Dim contarDigitos As Integer = 0
        If resto > 0 Then
            contarDigitos = Me.ContarDigitos(resto)
            Select Case contarDigitos
                Case 6
                    Dim centenaMil As Decimal = n Mod Math.Pow(10, contarDigitos)
                    monto += Me.GetCentenaMil(centenaMil)
                Case 5
                    Dim decenaMil As Decimal = n Mod Math.Pow(10, contarDigitos)
                    monto += Me.GetDecenaMil(decenaMil)
                Case 4
                    Dim unidadMil As Decimal = n Mod Math.Pow(10, contarDigitos)
                    monto += Me.GetUnidadMil(unidadMil)
                Case 3
                    Dim centena As Decimal = n Mod Math.Pow(10, contarDigitos)
                    monto += Me.GetCentena(centena)
                Case 2
                    Dim decena As Decimal = n Mod Math.Pow(10, contarDigitos)
                    monto += Me.GetDecena(decena)
                Case 1
                    Dim unidad As Decimal = n Mod Math.Pow(10, contarDigitos)
                    monto += Me.GetUnidad(unidad)
            End Select
        End If
        Return monto
    End Function

    Private Function GetCentenaMillon(ByVal n As Decimal) As String
        ' Ejemplo: 864123745
        Dim centenaMillon As Decimal = n \ 1000000
        Dim resto As Decimal = n Mod 1000000
        Dim monto As String = ""
        Dim contarDigitos As Integer = 0
        If resto > 0 Then
            contarDigitos = Me.ContarDigitos(resto)
        End If
        monto = Me.GetCentena(centenaMillon) & " MILLONES "
        Select Case contarDigitos
            Case 6
                Dim centenaMil As Decimal = n Mod Math.Pow(10, contarDigitos)
                monto += Me.GetCentenaMil(centenaMil)
                Dim asdasd = ""
            Case 5
                Dim decenaMil As Decimal = n Mod Math.Pow(10, contarDigitos)
                monto += Me.GetDecenaMil(decenaMil)
                Dim asdasd = ""
            Case 4
                Dim unidadMil As Decimal = n Mod Math.Pow(10, contarDigitos)
                monto += Me.GetUnidadMil(unidadMil)
                Dim asdasd = ""
            Case 3
                Dim centena As Decimal = n Mod Math.Pow(10, contarDigitos)
                monto += Me.GetCentena(centena)
                Dim asdasd = ""
            Case 2
                Dim decena As Decimal = n Mod Math.Pow(10, contarDigitos)
                monto += Me.GetDecena(decena)
                Dim asdasd = ""
            Case 1
                Dim unidad As Decimal = n Mod Math.Pow(10, contarDigitos)
                monto += Me.GetUnidad(unidad)
                Dim asdasd = ""
        End Select
        Return monto
    End Function

    Private Function ContarDigitos(ByVal n As Decimal) As Integer
        Return Math.Floor(Math.Log10(CDbl(n))) + 1
    End Function

End Class
