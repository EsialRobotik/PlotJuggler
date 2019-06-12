#include "AsservStream_uartDecoder.h"
#include <cstdio>
#include <functional>

#define CALL_MEMBER(object,ptrToMember)  ((object).*(ptrToMember))


AsservStream_uartDecoder::AsservStream_uartDecoder()
{
	currentState = &AsservStream_uartDecoder::synchroLookUp;
}


void AsservStream_uartDecoder::processBytes(uint8_t *buffer, unsigned int nbBytes)
{
	for(int i=0; i<nbBytes; i++)
		CALL_MEMBER(*this,currentState)(buffer[i]);
}


void AsservStream_uartDecoder::synchroLookUp(uint8_t byte)
{
	constexpr uint32_t synchroWord = 0xCAFED00D;
    static int nbSynchroByteFound = 0;

    if( byte == ((uint8_t*)&synchroWord)[nbSynchroByteFound] )
    {
    	nbSynchroByteFound++;
    }
    else
    {
    	isCurrentSampleValid = false;
    	nbSynchroByteFound = 0;
    	printf("drop..\n");
    }

   if( nbSynchroByteFound == sizeof(synchroWord))  // Synchro found !  
   {
		nbSynchroByteFound = 0;
		currentState =  &AsservStream_uartDecoder::getRemainingData;

		if( isCurrentSampleValid)
			decodedSampleQueue.push(currentDecodedSample);
   }
}

void AsservStream_uartDecoder::getRemainingData(uint8_t byte)
{
    static int nbByteRead = 0;
    static uint8_t *currentDecodedSamplePtr = (uint8_t*)&currentDecodedSample;
    currentDecodedSamplePtr[nbByteRead++] = byte;

    if(nbByteRead == sizeof(currentDecodedSample))
    {   
        nbByteRead = 0;
		currentState =  &AsservStream_uartDecoder::synchroLookUp;
		isCurrentSampleValid = true;
	}
}

bool AsservStream_uartDecoder::getDecodedSample(UsbStreamSample *sample)
{
	if(decodedSampleQueue.empty() || sample == NULL )
		return false;
	*sample = decodedSampleQueue.front();
	decodedSampleQueue.pop();
	return true;
}
