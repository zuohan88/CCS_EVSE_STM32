#ifndef PROJECTEXI_H_
#define PROJECTEXI_H_

#include "EXITypes.h"

#include "appHandEXIDatatypes.h"
#include "appHandEXIDatatypesEncoder.h"
#include "appHandEXIDatatypesDecoder.h"

#include "dinEXIDatatypes.h"
#include "dinEXIDatatypesEncoder.h"
#include "dinEXIDatatypesDecoder.h"

#define EXI_TRANSMIT_BUFFER_SIZE 256
//extern uint8_t exiTransmitBuffer[EXI_TRANSMIT_BUFFER_SIZE]; /* after encoding, here we find the exi byte stream. */

//extern struct appHandEXIDocument aphsDoc; /* The application handshake document. */
//extern struct appHandEXIDocument aphsDocEnc; /* The application handshake document. */
//extern struct appHandEXIDocument aphsDocDec; /* The application handshake document. */
//extern struct dinEXIDocument dinDocEnc; /* The DIN document. For encoder. */
//extern struct dinEXIDocument dinDocDec; /* The DIN document. For decoder. */
//extern bitstream_t global_streamEnc; /* The byte stream descriptor. */
//extern bitstream_t global_streamDec; /* The byte stream descriptor. */
//extern size_t global_streamEncPos; /* The position in the stream. */
//extern size_t global_streamDecPos; /* The position in the stream. */
//extern int g_errn;

#define SESSIONID_LEN 8

typedef union ssId{
	uint8_t 	u8t[SESSIONID_LEN];
	uint64_t 	u64t;
}ssId;
//extern ssId sessionId;


typedef struct{
	uint8_t exiTransmitBuffer[EXI_TRANSMIT_BUFFER_SIZE];
	struct dinEXIDocument dinDocEnc;
	struct dinEXIDocument dinDocDec;

	struct appHandEXIDocument aphsDoc;
	struct appHandEXIDocument aphsDocEnc;			//evse added
	struct appHandEXIDocument aphsDocDec;			//evse added

	bitstream_t global_streamEnc;
	bitstream_t global_streamDec;

	size_t global_streamEncPos;
	size_t global_streamDecPos;
	int g_errn;

	ssId sessionId;
	uint8_t ssID[SESSIONID_LEN];

}projectExiConnector;



/* Decoder functions *****************************************************************************************/

void projectExiConnector_encode_appHandExiDocument(projectExiConnector* const me);

#if defined(__cplusplus)
extern "C"
{
#endif
int projectExiConnector_decode_appHandExiDocument(projectExiConnector* const me);
  /* precondition: The global_streamDec.size and global_streamDec.data have been set to the byte array with EXI data. */
#if defined(__cplusplus)
}
#endif

#if defined(__cplusplus)
extern "C"
{
#endif
int projectExiConnector_decode_DinExiDocument(projectExiConnector* const me);
  /* precondition: The global_streamDec.size and global_streamDec.data have been set to the byte array with EXI data. */
#if defined(__cplusplus)
}
#endif


/* Encoder functions ****************************************************************************************/
#if defined(__cplusplus)
extern "C"
{
#endif
void projectExiConnector_prepare_DinExiDocument(projectExiConnector* const me);
/* before filling and encoding the dinDocEnc, we initialize here all its content. */
#if defined(__cplusplus)
}
#endif


#if defined(__cplusplus)
extern "C"
{
#endif
int projectExiConnector_encode_DinExiDocument(projectExiConnector* const me);
  /* precondition: dinDocEnc structure is filled. Output: global_stream.data and global_stream.pos. */  
#if defined(__cplusplus)
}
#endif

/* Test functions, just for experimentation *****************************************************************/
#if defined(__cplusplus)
extern "C"
{
#endif
void projectExiConnector_testEncode(projectExiConnector* const me);
#if defined(__cplusplus)
}
#endif


#if defined(__cplusplus)
extern "C"
{
#endif
int projectExiConnector_test(projectExiConnector* const me, int a);
#if defined(__cplusplus)
}
#endif

#endif
