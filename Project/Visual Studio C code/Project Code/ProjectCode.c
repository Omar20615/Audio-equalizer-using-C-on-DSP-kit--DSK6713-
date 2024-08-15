#include <stdio.h>
#include"LowPassCoefs.h"
#include"HighPassCoefs.h"
#include"BandPass4kto8kCoefs.h"
#include"BandPass9kto13kCoefs.h"
#include"BandPass13kto17kCoefs.h"
#include"tmwtypes.h"
#include <math.h>


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdint.h>
typedef struct {
	char chunkID[4];
	uint32_t chunkSize;
	char format[4];
	char subchunk1ID[4];
	uint32_t subchunk1Size;
	uint16_t audioFormat;
	uint16_t numChannels;
	uint32_t sampleRate;
	uint32_t byteRate;
	uint16_t blockAlign;
	uint16_t bitsPerSample;
	char subchunk2ID[4];
	uint32_t subchunk2Size;
} WAVHeader;

int audioread(const char* filename, float** audioData, int* numSamples, int* numChannels, int* sampleRate) {
	FILE* file = fopen(filename, "rb");
	if (file == NULL) {
		printf("Error opening file: %s\n", filename);
		return -1;
	}

	WAVHeader header;
	fread(&header, sizeof(WAVHeader), 1, file);

	if (header.audioFormat != 1) {
		printf("Unsupported audio format. Only PCM format (audioFormat=1) is supported.\n");
		fclose(file);
		return -1;
	}

	*numChannels = header.numChannels;
	*sampleRate = header.sampleRate;
	*numSamples = header.subchunk2Size / (header.numChannels * (header.bitsPerSample / 8));

	*audioData = (float*)malloc(sizeof(float) * (*numSamples) * (*numChannels));

	if (*audioData == NULL) {
		printf("Memory allocation error.\n");
		fclose(file);
		return -1;
	}

	int i, j;
	int16_t sample;
	float scale = 1.0f / (1 << (header.bitsPerSample - 1));

	/*for (i = 0; i < *numSamples; i++) {
		for (j = 0; j < *numChannels; j++) {
			fread(&sample, sizeof(int16_t), 1, file);
			(*audioData)[i * (*numChannels) + j] = scale * sample;
			printf("Sample[%d][%d]: %.4f\n", i, j, (*audioData)[i * (*numChannels) + j]);
		}
	}*/

	for (i = 0; i < *numSamples; i++) {
		fread(&sample, sizeof(int16_t), 1, file);
		(*audioData)[i] = scale * sample;
		//printf("Sample[%d]: %.4f\n", i, (*audioData)[i]);
	}

	fclose(file);

	return 0;
}


void filter(float* input, float* output, int numSamples, const real64_T* coefficients, int filterLength,int c) {
	int n = 0;
	double val1 = pow(10, (c / 20));
	for (n; n < numSamples; n++) {
		//output[n] = 0.0;
		int k = 0;
		for (k; k < filterLength; k++) {
			if (n - k >= 0) {
			
				
				//output[n] += (val1 * coefficients[k]) * input[n - k];
				output[n] += coefficients[k] * input[n - k];
				//printf("%f\n", outputSignal[n]);
			}
		}
	}
}

int audiowrite(const char* filename, const float* audioData, int numSamples, int numChannels, int sampleRate) {
	FILE* file = fopen(filename, "wb");
	if (file == NULL) {
		printf("Error opening file: %s\n", filename);
		return -1;
	}

	// Calculate the necessary values for the WAV header
	uint32_t chunkSize = numSamples * numChannels * sizeof(int16_t) + 36;
	uint32_t subchunk2Size = numSamples * numChannels * sizeof(int16_t);
	uint16_t audioFormat = 1; // PCM format
	uint16_t bitsPerSample = 16;
	uint16_t blockAlign = numChannels * sizeof(int16_t);
	uint32_t byteRate = sampleRate * blockAlign;

	// Write the WAV header
	WAVHeader header;
	memcpy(header.chunkID, "RIFF", 4);
	memcpy(header.format, "WAVE", 4);
	memcpy(header.subchunk1ID, "fmt ", 4);
	memcpy(header.subchunk2ID, "data", 4);
	header.chunkSize = chunkSize;
	header.subchunk1Size = 16; // Size of the fmt subchunk
	header.audioFormat = audioFormat;
	header.numChannels = numChannels;
	header.sampleRate = sampleRate;
	header.byteRate = byteRate;
	header.blockAlign = blockAlign;
	header.bitsPerSample = bitsPerSample;
	header.subchunk2Size = subchunk2Size;

	fwrite(&header, sizeof(WAVHeader), 1, file);

	// Write the audio data
	int i, j;
	int16_t sample;
	float scale = (1 << (bitsPerSample - 1)) - 1;

	for (i = 0; i < numSamples; i++) {
		for (j = 0; j < numChannels; j++) {
			sample = (int16_t)(audioData[i * numChannels + j] * scale);
			fwrite(&sample, sizeof(int16_t), 1, file);
		}
	}

	fclose(file);

	return 0;
}




#define bufferSize 168959
float outputSignal[bufferSize];
float outputSignal1[bufferSize];
float outputSignal2[bufferSize];
float outputSignal3[bufferSize];
float outputSignal4[bufferSize];
float outputSignal5[bufferSize];


int main() {
	const char* filename = "wavefile2.wav";
	float* audioData;
	int i;
	int numSamples, numChannels, sampleRate;

	int result = audioread(filename, &audioData, &numSamples, &numChannels, &sampleRate);

	if (result == 0) {
		printf("Audio file read successfully.\n");


	}
	else {
		printf("Failed to read audio file.\n");
	}
	int c1, c2, c3, c4, c5;
	c1 = 20;
	c2 = 1;
	c3 = 12;
	c4 = 5;
	c5 = 9;
	
	

	filter(audioData, outputSignal1, bufferSize, LowPass, sizeof(LowPass) / sizeof(LowPass[0]),c1);
	filter(audioData, outputSignal2, bufferSize, BandPass4kto8k, sizeof(BandPass4kto8k) / sizeof(BandPass4kto8k[0]),c2);
	filter(audioData, outputSignal3, bufferSize, BandPass9kto13k, sizeof(BandPass9kto13k) / sizeof(BandPass9kto13k[0]),c3);
	filter(audioData, outputSignal4, bufferSize, BandPass13kto17k, sizeof(BandPass13kto17k) / sizeof(BandPass13kto17k[0]),c4);
	filter(audioData, outputSignal5, bufferSize, HighPass, sizeof(HighPass) / sizeof(HighPass[0]),c5);
	//printf("Filtered signal:\n");
	
	for (i=0; i < bufferSize; i++) {
		outputSignal[i]=outputSignal1[i]+outputSignal2[i]+outputSignal3[i]+outputSignal4[i]+outputSignal5[i];
	}
	
	audiowrite("mywave.wav", outputSignal, numSamples, numChannels, sampleRate);
	printf("Audio writen successfuly\n");

	free(audioData);

	return 0;
}