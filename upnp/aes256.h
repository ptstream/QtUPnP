/*
 This file defined an EXAMPLE to create hash key and iv for AES 256 encryption.
 This keys must be as invisible as possible because they are used to encrypt and decrypt
 the tokens requisite by the authentification on the remote servers.
 E.g. for google drive, client_id, client_secret, access_token and refresh_token are encrypted
 to hide these values. They are decrypted only when they are used.

 The method below is an example to produce AES 256 keys.
 You can create your own method. Just an array of char (one for key one for iv) is important.

 The method below use 2 strings of 32 charaters.

 AES 256 Hash key. Each character is converted like:
 'ascii' -> hexadecimal value.
 '0'     -> 0x30
 '5'     -> 0x35
 ...
 'c'     -> 0x63
 ...
 QByteArray key("35314c8e64084db3f4bd6cbf616d4fee");
  char key[] = { 0x30, 0x35, 0x33, 0x31, 0x34, 0x63, 0x38, 0x65,
                 0x36, 0x34, 0x30, 0x38, 0x34, 0x64, 0x62, 0x33,
                 0x66, 0x34, 0x62, 0x64, 0x36, 0x63, 0x62, 0x66,
                 0x36, 0x31, 0x36, 0x64, 0x34, 0x66, 0x65, 0x65, 0x00 };

 AES 256 IV. Same method.
  QByteArray iv ("e5316ab7 f7994bd2 a2422fab b31bb3d1");
  char iv[] = { 0x65, 0x35, 0x33, 0x31, 0x36, 0x61, 0x62, 0x37,
                0x66, 0x35, 0x33, 0x31, 0x36, 0x61, 0x62, 0x37,
                0x61, 0x32, 0x34, 0x32, 0x32, 0x66, 0x61, 0x62,
                0x62, 0x33, 0x33, 0x62, 0x62, 0x33, 0x64, 0x31, 0x00 };

 ukey contains the hash key and iv used by AES 256 encryption.
 key[0], key[1], ... , key[7],  IV[7] -> quint64
 key[0], key[8], ... , key[15], IV[15] -> quint64
 ....
 key[24], key[25], ... , key[31], IV[31] -> quint64

 Apply same metho to iv.
 */

// This part is to not have visible following characters in the exeucutable.
quint64 ukey[] = { 0x3035333134633865ul,
                   0x3634303834646233ul,
                   0x6634626436636266ul,
                   0x3631366434666565ul,
                 };

quint64 uiv[] = { 0x6535333136616237ul,
                  0x6635333136616237ul,
                  0x6132343232666162ul,
                  0x6233336262336431ul,
                 };

// Compute key and iv used by COAuth2::characters. The quint64 must be converted to char.
// AIVCtrl use a similar but more complex algorithm to create key and iv.
// The following characters exists for a very short time in memory.
char key[33], iv[33];
for (unsigned i = 0, k = 0; i < sizeof (ukey) / sizeof (quint64); ++i)
{
  for (unsigned j = 0; j < 8; ++j)
  {
    int shift = 8 * j;
    key[k]    = static_cast<char>((ukey[i] >> shift) & 0xff);
    iv[k]     = static_cast<char>((uiv[i] >> shift) & 0xff);
    ++k;
  }
}

key[32] = iv[32] = '\0';


