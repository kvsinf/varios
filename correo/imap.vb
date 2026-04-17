Dim mail As TcpClient = New TcpClient
            mail.Connect(Me.txtServerName.Text.Trim, Me.txtPort.Text.Trim)
            ssl_stream = New SslStream(mail.GetStream)
            ssl_stream.AuthenticateAsClient(Me.txtServerName.Text.Trim)
            bytes = ssl_stream.Read(buffer, 0, buffer.Length)
            Dim response As String = Encoding.ASCII.GetString(buffer, 0, bytes)
            If response.StartsWith("* OK") Then
                Dim AuthXoauth2 As String = GetAuthXoauth2()
                If AuthXoauth2 <> "" Then
                    response = Me.cmd("2 AUTHENTICATE XOAUTH2 " & AuthXoauth2 & vbCrLf)
                    If response.StartsWith("2 OK") Then
                        ' Cantidad de correos 
                        Dim str As String = "3 STATUS " & """" & Me.txtFolder.Text.Trim & """" & " (UNSEEN)" & vbCrLf
                        Dim CantidadCorreos As Integer = Me.GetCantidadCorreos(str)
                        MessageBox.Show("Authentication successful. Total emails: " & CantidadCorreos, "Success", MessageBoxButtons.OK, MessageBoxIcon.Information)
                    Else
                        ' Autenticación IMAP Error
                    End If
                Else
                    MessageBox.Show("Error while obtaining Access Token.", "Authentication Error", MessageBoxButtons.OK, MessageBoxIcon.Error)
                End If
            Else
                ' El servidor o el puerto no corresponden 
                MessageBox.Show("The server or port do not correspond.", "Authentication Error", MessageBoxButtons.OK, MessageBoxIcon.Error)
            End If
        Catch ex As Exception
            MessageBox.Show(ex.ToString, "", MessageBoxButtons.OK, MessageBoxIcon.Error)
        End Try
