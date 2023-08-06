//My game creation Playground

#include <stdlib.h>
#include <SDL.h>
#include <SDL2/SDL_events.h>

#include <math.h>

#define Pi32 3.14159265358979f

#define internal static
#define global_variable static
#define local_persist static

struct sdl_offscreen_buffer {
  void *Memory;
  int Width;
  int Height;
  SDL_Texture *Texture;
  int Pitch;
};

struct sdl_window_dimension {
  int Height;
  int Width;
};

struct colors {
  int Blue=0;
  int Green=0;
  int Red=0;
};

struct sdl_sound_output {
  int SamplesPerSecond;
  int ToneHz;
  int16_t ToneVolume;
  uint32_t RunningSampleIndex;
  int WavePeriod;
  int BytesPerSample;
  int SecondaryBufferSize;
  float tSine;
  int LatencySampleCount;
};

global_variable colors COLS;
global_variable sdl_offscreen_buffer GlobalBackBuffer;

global_variable short ChangeWave = -2561;

sdl_sound_output SoundOutput = {};

sdl_window_dimension
SDLGetWindowDimension(SDL_Window *Window)
{
  sdl_window_dimension Result;

  SDL_GetWindowSize(Window, &Result.Width, &Result.Height);

  return(Result);
}

internal void
RenderWeirdGradient(sdl_offscreen_buffer *Buffer,int *BlueOffset, int *GreenOffset, int *RedOffset)
{
  int Width = Buffer->Width;
  int Height = Buffer->Height;

  uint8_t *Row = (uint8_t *)Buffer->Memory;
  for( int Y = 0;
       Y < Buffer->Height;
       Y++)
    {
      uint32_t *Pixel = (uint32_t *)Row;
      for( int X = 0;
           X < Buffer->Width;
           X++)
        {
          uint8_t Blue = (X + *BlueOffset);
          uint8_t Green = (Y + *GreenOffset);
          uint8_t Red = (((X + Y)/2) + *RedOffset);

          *Pixel++ = ((Green << 8) | Blue | (Red << 16));
        }

      Row += Buffer->Pitch;
    }
}

internal void
SDLResizeTexture(sdl_offscreen_buffer *Buffer,SDL_Renderer *Renderer,int Width,int Height)
{
  int BytesPerPixel = 4;
  if(Buffer->Memory)
    {
      free(Buffer->Memory);
    }
  
  if(Buffer->Texture)
    {
      SDL_DestroyTexture(Buffer->Texture);
    }
  
  Buffer->Texture = SDL_CreateTexture(Renderer,
                                      SDL_PIXELFORMAT_ARGB8888,
                                      SDL_TEXTUREACCESS_STREAMING,
                                      Width,
                                      Height);
  Buffer->Width = Width;
  Buffer->Height = Height;
  Buffer->Pitch = Width * BytesPerPixel;
  Buffer->Memory = malloc(Width * Height * BytesPerPixel);
}

internal void
SDLUpdateWindow(sdl_offscreen_buffer *Buffer,SDL_Window *Window,SDL_Renderer *Renderer)
{
  SDL_UpdateTexture(Buffer->Texture,
                    0,
                    Buffer->Memory,
                    Buffer->Pitch);
  SDL_RenderCopy(Renderer,
                 Buffer->Texture,
                 0,
                 0);
  SDL_RenderPresent(Renderer);
}

internal void
GenerateDiffSound(void)
{
  SoundOutput.ToneHz = 512 + (int)(256.0f*((float)ChangeWave / 3000.0f));
  SoundOutput.WavePeriod = SoundOutput.SamplesPerSecond / SoundOutput.ToneHz;
}

internal void
SDLInitAudio (int32_t SamplesPerSecond, int32_t BufferSize)
{
  SDL_AudioSpec AudioSettings = {0};

  AudioSettings.freq = SamplesPerSecond;
  AudioSettings.format = AUDIO_S16;
  AudioSettings.channels = 2;
  AudioSettings.samples = BufferSize;

  SDL_OpenAudio(&AudioSettings,0);

  printf("Initialized audio device at frequency %d Hz, %d Channels,buffersize %d\n", AudioSettings.freq,AudioSettings.channels,AudioSettings.samples);

  if (AudioSettings.format != AUDIO_S16)
    {
      printf("we didn't get s16 sample\n");
      SDL_CloseAudio();
    }
}

internal void
SDLFillSoundBuffer(sdl_sound_output *SoundOutput, int BytesToLock,int BytesToWrite)
{
  int SampleCount = BytesToWrite / SoundOutput->BytesPerSample;
  void *AudioBuffer = malloc(BytesToWrite);
  int16_t *SampleOut = (int16_t*)AudioBuffer;
  for (int SampleIndex = 0;
       SampleIndex < SampleCount;
       ++SampleIndex)
    {
      float SineValue = sinf(SoundOutput->tSine);
      int16_t SampleValue = (int16_t)(SineValue * SoundOutput->ToneVolume);
      *SampleOut++ = SampleValue;
      *SampleOut++ = SampleValue;

      SoundOutput->tSine += 2.0f*Pi32*1.0f/(float)SoundOutput->WavePeriod;
      ++SoundOutput->RunningSampleIndex;
    }
  SDL_QueueAudio(1,AudioBuffer,BytesToWrite);

  free(AudioBuffer);
}

bool HandleEvent(SDL_Event *Event)
{
  bool ShouldClose = false;
  

  switch(Event->type)
    {
    case SDL_QUIT:
      {
        ShouldClose = true;
      } break;

    case SDL_KEYDOWN:
    case SDL_KEYUP:
      {
        SDL_Keycode KeyCode = Event->key.keysym.sym;
        bool IsDown = (Event->key.state == SDL_PRESSED);
        bool WasDown = false;

        bool AltKeyDown = (Event->key.keysym.mod & KMOD_ALT);
        
        if(Event->key.state == SDL_RELEASED)
          {
            WasDown = true;
          }
        else if ( Event->key.repeat != 0)
          {
            WasDown = true;
          }
            if(KeyCode == SDLK_a)
              {
                (COLS.Blue) += 40;
                (COLS.Red) += 40;

                GenerateDiffSound();
                ChangeWave += 100;
              }
            else if(KeyCode == SDLK_w)
              {
                (COLS.Green) += 30;                
                (COLS.Red) += 30;

                GenerateDiffSound();
                ChangeWave += 100;
              }
            else if(KeyCode == SDLK_d)
              {
                (COLS.Blue) -= 40;
                (COLS.Red) -= 40;

                GenerateDiffSound();
                ChangeWave -= 100;
              }
            else if(KeyCode == SDLK_s)
              {
                (COLS.Green) -= 30;                
                (COLS.Red) -= 30;

                GenerateDiffSound();
                ChangeWave -= 100;
              }
            else if(KeyCode == SDLK_e)
              {
                
              }
            else if(KeyCode == SDLK_UP)
              {
                
              }
            else if(KeyCode == SDLK_LEFT)
              {
                
              }
            else if(KeyCode == SDLK_DOWN)
              {

              }
            else if(KeyCode == SDLK_RIGHT)
              {
                
              }
            else if(KeyCode == SDLK_ESCAPE)
              {
                printf("ESCAPE: ");
                if(IsDown)
                  {
                    printf("IsDown ");
                  }
                if(WasDown)
                  {
                    printf("WasDown ");
                  }
                printf("\n");
              }
            else if(KeyCode == SDLK_SPACE)
              {
                
              }
            else if((KeyCode == SDLK_F4) && AltKeyDown)
              {
                ShouldClose = true;
              }
            
      }break;
      
    case SDL_WINDOWEVENT:
      {
        switch(Event->window.event)
          {
          case SDL_WINDOWEVENT_RESIZED:
            {
              SDL_Window *Window = SDL_GetWindowFromID(Event->window.windowID);
              SDL_Renderer *Renderer = SDL_GetRenderer(Window);
              printf("SDL_WINDOWEVENT_RESIZED (%d,%d)\n",Event->window.data1,Event->window.data2);
            }break;
            
          case SDL_WINDOWEVENT_EXPOSED:
            {
              SDL_Window *Window = SDL_GetWindowFromID(Event->window.windowID);
              SDL_Renderer *Renderer = SDL_GetRenderer(Window);
              SDLUpdateWindow(&GlobalBackBuffer,Window, Renderer);
            }break;
            
          case SDL_WINDOWEVENT_FOCUS_GAINED:
            {
              printf("SDL_WINDOW_FOCUS_GAINED\n");
            }break;
          }
      }break;

    }
    
  return(ShouldClose);
}


int main(int argc, char *argv[])
{
  SDL_Window *Window;
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

  uint64_t PerfCountFrequency = SDL_GetPerformanceFrequency();

  // Create out window.
  Window = SDL_CreateWindow("God Game",
                            SDL_WINDOWPOS_UNDEFINED,
                            SDL_WINDOWPOS_UNDEFINED,
                            640,
                            480,
                            SDL_WINDOW_RESIZABLE);
  if(Window)
    {
      // Create a "Renderer" for our window.
      SDL_Renderer *Renderer = SDL_CreateRenderer(Window,
                                                  -1,
                                                  0);
      if(Renderer)
        {
          bool Running = true;
          sdl_window_dimension Dimension = SDLGetWindowDimension(Window);
          SDLResizeTexture(&GlobalBackBuffer,Renderer, Dimension.Width, Dimension.Height);
          
          SoundOutput.SamplesPerSecond = 48000;
          SoundOutput.ToneHz = 256;
          SoundOutput.ToneVolume = 3000;
          SoundOutput.WavePeriod = SoundOutput.SamplesPerSecond / SoundOutput.ToneHz;
          SoundOutput.BytesPerSample = sizeof(int16_t) * 2;
          SoundOutput.LatencySampleCount = SoundOutput.SamplesPerSecond / 15;
          
          // Open Audio Device
          SDLInitAudio(SoundOutput.SamplesPerSecond, SoundOutput.SamplesPerSecond * SoundOutput.BytesPerSample / 60);
          SDLFillSoundBuffer(&SoundOutput, 0, SoundOutput.LatencySampleCount * SoundOutput.BytesPerSample);
          SDL_PauseAudio(0);

          uint64_t LastCounter = SDL_GetPerformanceCounter();
          uint64_t LastCycleCount = _rdtsc();
          
          while(Running)
            {
              SDL_Event Event;
              while(SDL_PollEvent(&Event))
                {
                  if(HandleEvent(&Event))
                    {
                      Running = false;
                    }
                }
                  RenderWeirdGradient(&GlobalBackBuffer,&COLS.Blue,&COLS.Green,&COLS.Red);


                  
                  // Soundtest
                  int TargetQueueBytes = SoundOutput.LatencySampleCount * SoundOutput.BytesPerSample;
                  int BytesToWrite = TargetQueueBytes - SDL_GetQueuedAudioSize(1);
                  SDLFillSoundBuffer(&SoundOutput, 0, BytesToWrite);
                  
                  SDLUpdateWindow(&GlobalBackBuffer,Window,Renderer);
                  uint64_t EndCyclecount = _rdtsc();
                  uint64_t EndCounter = SDL_GetPerformanceCounter();
                  uint64_t CounterElapsed = EndCounter - LastCounter;
                  uint64_t CyclesElapsed = EndCyclecount- LastCycleCount;

                  double MSPerFrame = (((1000.0f * (double)CounterElapsed) / (double)PerfCountFrequency));
                  double FPS = (double)PerfCountFrequency / (double)CounterElapsed;
                  double MCPF = ((double)CyclesElapsed / (1000.0f * 1000.0f));
                  printf("MSPerFrame = %0.2f ms/f, FPS =  %.02f /s, MCPF =  %0.2f mc/f\n",MSPerFrame,FPS,MCPF);

                  LastCycleCount = EndCyclecount;
                  LastCounter = EndCounter;
            }
        }
    }
    
  SDL_Quit();
  return 0;
}
