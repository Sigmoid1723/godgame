#include <stdlib.h>
#include <SDL.h>
#include <SDL2/SDL_events.h>

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

global_variable sdl_offscreen_buffer GlobalBackBuffer;
global_variable colors COLS;

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
SDLAudioCallback(void *UserData, uint8_t *AudioData, int Length)
{
  // clear audio buffer to silence.
  memset(AudioData,0, Length);
}

internal void
SDLInitAudio (int32_t SamplesPerSecond, int32_t BufferSize)
{
  SDL_AudioSpec AudioSettings = {0};

  AudioSettings.freq = SamplesPerSecond;
  AudioSettings.format = AUDIO_S16;
  AudioSettings.channels = 2;
  AudioSettings.samples = BufferSize;
  AudioSettings.callback = &SDLAudioCallback;

  SDL_OpenAudio(&AudioSettings,0);

  printf("Initialized audio device at frequency %d Hz, %d Channels\n", AudioSettings.freq,AudioSettings.channels);

  if (AudioSettings.format != AUDIO_S16)
    {
      printf("we didn't get s16 sample\n");
      SDL_CloseAudio();
    }

  SDL_PauseAudio(0);
}

bool HandleEvent(SDL_Event *Event)
{
  bool ShouldClose = false;
  

  switch(Event->type)
    {
    case SDL_QUIT:
      {
        printf("SDL QUIT\n");
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
              }
            else if(KeyCode == SDLK_w)
              {
                (COLS.Green) += 30;                
                (COLS.Red) += 30;                
              }
            else if(KeyCode == SDLK_d)
              {
                (COLS.Blue) -= 40;
                (COLS.Red) -= 40;
              }
            else if(KeyCode == SDLK_s)
              {
                (COLS.Green) -= 30;                
                (COLS.Red) -= 30;                
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
                printf("SDL QUIT\n");
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

  // Open our audio.
  SDLInitAudio(48000, 4096);

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
                  SDLUpdateWindow(&GlobalBackBuffer,Window,Renderer);
            }
        }
    }
    
  SDL_Quit();
  return 0;
}
