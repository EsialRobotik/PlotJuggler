#ifndef ASSERV_STREAM_DECODER_H
#define ASSERV_STREAM_DECODER_H
#include <cstdint>
#include <queue>


typedef struct
{
	uint32_t timestamp;
	float value1;
	float value2;
	float value3;
	float value4;
	float value5;
	float value6;
	float value7;
	float value8;
	float value9;
	float value10;
	float value11;
	float value12;
	float value13;
	float value14;
	float value15;
	float value16;
	float value17;
	float value18;
	float value19;
	float value20;
	float value21;
	float value22;
	float value23;
	float value24;
	float value25;
	float value26;
	float value27;
}  __attribute__((packed)) UsbStreamSample;


class  AsservStream_uartDecoder
{
public:

	AsservStream_uartDecoder();
    virtual ~AsservStream_uartDecoder(){};
	void processBytes(uint8_t *buffer, unsigned int nbBytes);

	bool getDecodedSample(UsbStreamSample *sample);

	uint8_t configBuffer[512];
	int nbValues;
	bool configAvailable = false;

private:
	UsbStreamSample currentDecodedSample;
	bool isCurrentSampleValid = false;

	std::queue<UsbStreamSample> decodedSampleQueue;

	void synchroLookUp(uint8_t byte);
	void getRemainingData(uint8_t byte);
	void getRemainingConfig(uint8_t byte);

	typedef void (AsservStream_uartDecoder::*stateFunction)(uint8_t byte);
	stateFunction currentState;
};

#endif

