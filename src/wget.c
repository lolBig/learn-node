#include <header-parser.h>
#include "learn_node.h"
#include <openssl/ssl.h>

int verify_cert(const char* pem_c_str)
{
  BIO *bio_mem = BIO_new(BIO_s_mem());
  BIO_puts(bio_mem, pem_c_str);
  X509 * x509 = PEM_read_bio_X509(bio_mem, NULL, NULL, NULL);

  EVP_PKEY *pkey=X509_get_pubkey(x509);
  int r= X509_verify(x509, pkey);
  EVP_PKEY_free(pkey);

  BIO_free(bio_mem);
  X509_free(x509);
  return r;
}

int main(int argc, char **argv) {
  OpenSSL_add_all_algorithms();

  const char c0[] = "-----BEGIN CERTIFICATE-----" "\n"
          "MIIDIjCCAgoCCQCE8H4/ymXyrzANBgkqhkiG9w0BAQUFADBTMQswCQYDVQQGEwJV" "\n"
          "UzENMAsGA1UECAwEVXRhaDENMAsGA1UEBwwET3JlbTEQMA4GA1UECgwHT3JnTmFt" "\n"
          "ZTEUMBIGA1UEAwwLZXhhbXBsZS5jb20wHhcNMTMwNDEwMjIxMjE5WhcNMTQwNDEw" "\n"
          "MjIxMjE5WjBTMQswCQYDVQQGEwJVUzENMAsGA1UECAwEVXRhaDENMAsGA1UEBwwE" "\n"
          "T3JlbTEQMA4GA1UECgwHT3JnTmFtZTEUMBIGA1UEAwwLZXhhbXBsZS5jb20wggEi" "\n"
          "MA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQCuzA1sONmXPc6aMt+cZExA37OZ" "\n"
          "kpVlfMCQUy8tTvqSs81F0DeTUGqq8ACdXT9iMlvENQ04xrtTEHPJcY93cAsaLowX" "\n"
          "6pB1y1F+8Jj+rrOwmKjBM9EI0/M9TCS94IGqTcPwgQt1d+XOZ+EdL63SkTQtNFHH" "\n"
          "hGs+g9Q+zeSM0uD7WgVxJPWezjnzQUis4j9ICXwMpuAMcmTqmxSqTzOQZAINJ9Hv" "\n"
          "sazPMVKs+JPEZvCfP0r61d1C8WLE7QF4nmdmWUTaBO+92piqQSeF7rK3bWmCxJNX" "\n"
          "8BFQd6h8g4XviMrybSwzf3JgM2Wxw27Vo9EADZ5Om8EjNPvB2UIbAokCOBN7AgMB" "\n"
          "AAEwDQYJKoZIhvcNAQEFBQADggEBAHhm2J8+Dg91S1b/i9LEpn41QSMpyyonzxqo" "\n"
          "o45CzJAuV5qN6x7FMBXB+1e+Na4Qn5K/8fJ8Z2M6jIO2MD+gB+ftVY830aN8cm+i" "\n"
          "/Cu/iUgB9kaSDLBUZvwu2uSEyDFwdxgmF5jK2BECNTP5A99WtL3w0dE60w5Bq23L" "\n"
          "Ivzd7XZF1orR9gJYOGHNK2s3S1vJQLBRvfRi78wfl25jyaZ2JWKGguFpQq1zJkrY" "\n"
          "PeCGvx+54fTOTi1PZcL4+xYfA//dvB1DnlHwpNSKnWkcNI5VK6IpDfBlh4ZjB3I3" "\n"
          "h6v6zOyvgOcvTXBHmzPsfMym1AmFNTv9/bRlwrKUlGGPaRwSEKU=" "\n"
          "-----END CERTIFICATE-----";
  printf("%d", verify_cert(c0));
  return 0;
}