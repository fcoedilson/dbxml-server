#include <gnutls/gnutls.h>
#include "start.h"
#include "../protocol/protocol.h"

void handle_session(DbXmlSessionData *session) {

  gnutls_init(&(session->Encryption.session), GNUTLS_SERVER);

  gnutls_priority_set_direct(session->Encryption.session,
                             "NORMAL:+ANON-DH", NULL);

  gnutls_priority_set(session->Encryption.session,
                      session->options->Encryption.priority_cache);

  gnutls_credentials_set(session->Encryption.session,
                         GNUTLS_CRD_CERTIFICATE,
                         session->options->Encryption.x509_cred);

  gnutls_certificate_server_set_request(session->Encryption.session,
                                        GNUTLS_CERT_REQUEST);

  gnutls_transport_set_ptr(session->Encryption.session,
                           (gnutls_transport_ptr_t)(session->Connection.client_fd));

  int ret = gnutls_handshake(session->Encryption.session);

  if (ret < 0) {
    gnutls_deinit(session->Encryption.session);
    LOG_INFO("*** Handshake has failed (%s)\n\n",
             gnutls_strerror(ret));

  } else {
    LOG_INFO("*** Handshake succeeded\n");
    
    // hand the session to the protocol handling code.
    protocol_start_session(session);

    gnutls_bye(session->Encryption.session,
               GNUTLS_SHUT_WR);
    gnutls_deinit(session->Encryption.session);
  }

}
