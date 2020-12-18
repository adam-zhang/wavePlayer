/**
*test.c
*
*注意：这个例子在Ubuntu 12.04.1环境下编译运行成功。
*
*/
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <iomanip>
#include <memory>
#include <alsa/asoundlib.h>

using namespace std;

static std::stringstream readFile(const std::string& fileName)
{
	//std::ifstream file(fileName, ios_base::in|ios_base::binary);
	std::ifstream file(fileName);
	std::stringstream buffer;
	buffer << file.rdbuf();
	return buffer;
}

void writeData(const std::string& note, const char* data, int length)
{
	std::cout << note << ":";
	for(auto i = 0; i != length; ++i)
	{
		std::cout << std::setw(2) << std::setfill('0') << std::hex << (unsigned int)(unsigned char)data[i] << " ";
	}
	std::cout << "\n";
}

void playSound(const std::string& fileName);

int main(int argc, char *argv[])
{
	if (argc != 3)
	{
		cout << "usage:\n"
			"\tsound_player <sound_file> <times>";
		return -1;
	}
	int count = atoi(argv[2]);
	for(int i = 0; i != count; ++i)
	{
		playSound(argv[1]);
	}
	return 0;
}

void deleter(FILE* p)
{
	fclose(p);
}


void playSound(const std::string& fileName)
{
	auto fp = std::unique_ptr<FILE, decltype(fclose)*>(fopen(fileName.c_str(), "rb"), fclose);
	if(fp == NULL)
		return ;
	fseek(fp.get(), 100, SEEK_SET);
	
	snd_pcm_t* playback_handle;
	int ret = snd_pcm_open(&playback_handle, "default", SND_PCM_STREAM_PLAYBACK, 0);
	auto handle = std::unique_ptr<snd_pcm_t, decltype(snd_pcm_close)*>(playback_handle, snd_pcm_close);
	if (ret < 0) {
		perror("snd_pcm_open");
		return;
	}
	
	snd_pcm_hw_params_t *hw_params;
	ret = snd_pcm_hw_params_malloc(&hw_params);
	auto params = std::unique_ptr<snd_pcm_hw_params_t, decltype(snd_pcm_hw_params_free)*>(hw_params, snd_pcm_hw_params_free);
	if (ret < 0) {
		perror("snd_pcm_hw_params_malloc");
		return;
	}
	ret = snd_pcm_hw_params_any(handle.get(), params.get());
	if (ret < 0) {
		perror("snd_pcm_hw_params_any");
		return;
	}
	ret = snd_pcm_hw_params_set_access(handle.get(), params.get(), SND_PCM_ACCESS_RW_INTERLEAVED);
	if (ret < 0) {
		perror("snd_pcm_hw_params_set_access");
		return;
	}
	ret = snd_pcm_hw_params_set_format(handle.get(), params.get(), SND_PCM_FORMAT_S16_LE);
	if (ret < 0) {
		perror("snd_pcm_hw_params_set_format");
		return;
	}

	unsigned int val = 16000;
	int dir = 0;
	ret = snd_pcm_hw_params_set_rate_near(handle.get(), params.get(), &val, &dir);
	if (ret < 0) 
	{
		perror("snd_pcm_hw_params_set_rate_near");
		return;
	}
	ret = snd_pcm_hw_params_set_channels(handle.get(), params.get(), 1);
	if (ret < 0) {
		perror("snd_pcm_hw_params_set_channels");
		return;
	}

	snd_pcm_uframes_t frames = 32;
	snd_pcm_uframes_t periodsize = frames ;
	ret = snd_pcm_hw_params_set_buffer_size_near(handle.get(), params.get(), &periodsize);
	if (ret < 0)
	{
		printf("Unable to set buffer size %li : %s\n", frames * 2, snd_strerror(ret));

	}
	periodsize /= 2;

	ret = snd_pcm_hw_params_set_period_size_near(handle.get(), params.get(), &periodsize, 0);
	if (ret < 0)
	{
		printf("Unable to set period size %li : %s\n", periodsize,  snd_strerror(ret));
	}

	ret = snd_pcm_hw_params(handle.get(), params.get());
	if (ret < 0) {
		perror("snd_pcm_hw_params");
		return;
	}

	snd_pcm_hw_params_get_period_size(params.get(), &frames, &dir);

	int size = frames * 2; 
	vector<char> buffer(size);
	while (true)
	{
		ret = fread(&buffer[0], 1, size, fp.get());
		if(ret == 0)
		{
			return;
		}
		else if (ret != size)
		{
		}
		while(ret = snd_pcm_writei(handle.get(), &buffer[0], frames) < 0)
		{
			if (ret == -EPIPE)
			{
				fprintf(stderr, "underrun occurred\n");
				snd_pcm_prepare(handle.get());
			}
			else if (ret < 0)
			{
				fprintf(stderr, "error from writei: %s\n", snd_strerror(ret));
			}
		}

	}
}
//
////注意：编译的时候应该保持“gcc -o test test.c -L. -lasound”的格式，运行的时候应该保持"./test clip2.wav"这种格式。
