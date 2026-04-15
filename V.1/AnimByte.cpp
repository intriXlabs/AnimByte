// animbyte is a opensource 1byte character based rendering engine to provide high speed animation with easy api access

#include <iostream>
#include <vector>
#include <stdlib.h>
#include <unistd.h>

class AnimByte{
public:
    int Initialise(int width, int height){
        Allocate_Buffer(width, height);
        return 0;
    }

    int Set_Char(int row, int col, char ch){
        row=row-1,col=col-1;
        if(row < 0 || row >= Height || col < 0 || col >= Width){
            return -1;
        }

        if(Buffer[row * Width + col] == ch)
            return -1;

        Buffer[row * Width + col] = ch;
        List.push_back({row, col});
        return 0;
    }

    int Render_Frame(){
        Generate_Frame();   // build Frame from Buffer
        if(Frame.empty())
            return -1;
        render(Frame);
        return 0;
    }

    int Frame_Clean(){
        for(int i = 0; i < List.size(); i++){
            Set_Char(List[i].row, List[i].col, ' ');
        }
        return 0;
    }

private:
    struct Clean_List{
        int row, col;
    };
    char *Buffer;
    int Width, Height;
    std::string Frame;
    std::vector <Clean_List> List;

    int Allocate_Buffer(int width, int height){
        Width = width;
        Height = height;
        Buffer = new char[Width * Height];
        for(int i = 0; i < Width * Height; i++){
            Buffer[i] = ' ';
        }
        return 0;
    }

    int Generate_Frame(){
        Frame.clear();

        for(int i = 0; i < Width * Height; i++){
            Frame += Buffer[i];

            // insert newline at end of each row
            if((i + 1) % Width == 0){
                Frame += '\n';
            }
        }

        return 0;
    }

    int render(const std::string &frame){
        const char *home = "\033[H";
        write(1, home, 3);
        write(1, frame.c_str(), frame.size());
        return 0;
    }
};
