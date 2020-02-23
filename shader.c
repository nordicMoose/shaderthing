#include "engine/include/core/application.h"
#include "engine/external/glad/glad/glad.h"
#include "engine/include/core/keycodes.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "sloader.c"

#define F_NOZERO 1
#define F_NONEGATIVE 2
int _StringToLongB(const char* string, long* out_value, int flags, int base)
{
    char* endptr;
    long value = strtol(string,&endptr,base);
    if((endptr == NULL || *endptr == '\0') && (value || !(flags & F_NOZERO)) && ((value<0)? !(flags & F_NONEGATIVE) : 1))
    {
        *out_value = value;
        return 1;
    }
    return 0;
}

char* ReadFile(const char* filename)
{
    if(!FileExists(filename)){
        printf("File does not exist: %s\n", filename);
        return 0;        
    }
    FILE* file = fopen(filename,"rb");
    if (file == 0){
        printf("Could not open file: %s\n", filename);
        return 0;
    }
    char* result;
    fseek(file, 0, SEEK_END);
    size_t len = ftell(file);
    fseek(file, 0, SEEK_SET);
    result = malloc(len+1);
    fread(result, 1, len, file);
    result[len] = 0;
    fclose(file);
    return result;
}

const char* vertexShader = "#version 460 core\n"
"layout (location = 0) in vec3 VertexPostion;"
"void main()"
"{"
"    gl_Position = vec4(VertexPostion,1.0f);"
"}";

/*Variables provided:
 - Mouse position and button states (x=x,y=y,z=lb,w=rb)
 - Window resolution
 - Delta time
 - Total time since application start
*/

const char* fragmentShaderStub ="#version 460 core\n"
                                "uniform vec4 MousePosition;"
                                "uniform float DeltaTime;"
                                "uniform float TotalTime;"
                                "uniform vec2 Resolution;"
                                "out vec4 DiffuseColor;"
                                "layout(pixel_center_integer) in vec4 gl_FragCoord;"
                                "vec4 FragCoord = gl_FragCoord;\n";

const char* attribs[] = {"VertexPosition"};

GLuint shaderProgram;

//drawing quad
GLfloat sqvert[] = {
    1.0,1.0,0,
    1.0,-1.0,0,
    -1.0,1.0,0,

    1.0,-1.0,0,
    -1.0,-1.0,0,
    -1.0,1.0,0
};

GLfloat uniformDeltaTime = 0;
GLfloat uniformTotalTime = 0;
GLfloat uniformWindowResolution[2] = {0};
GLfloat uniformMousePosition[4] = {0};

GLint uDeltaTimeLocation = 0;
GLint uTotalTimeLocation = 0;
GLint uWindowResolutionLocation = 0;
GLint uMousePositionLocation = 0;

char* shaderFile;

//quad with z -1..1
//GLfloat sqvert[] = {
//    1.0,1.0,1.0,
//    1.0,-1.0,-1.0,
//    -1.0,1.0,1.0,
//
//    1.0,-1.0,-1.0,
//    -1.0,-1.0,-1.0,
//    -1.0,1.0,1.0
//};

GLuint res;

void Start()
{
    res = 123;
    shaderProgram = 0;

    char* fshaderstr = ReadFile(shaderFile);
    if(fshaderstr == 0)
    {
        printf("Failed to load shader, exiting...");
        ExitApplication();
        return;
    }

    char* fragmentShader = calloc(strlen(fshaderstr) + strlen(fragmentShaderStub) + 1, sizeof(char));
    strcpy(fragmentShader,fragmentShaderStub);
    strcat(fragmentShader,fshaderstr);

    shaderProgram = ConstructShader(vertexShader,fragmentShader,attribs,1);

    if(shaderProgram == 0)
    {
        printf("Failed to compile shader, exiting...");
        ExitApplication();
        return;
    }

    free(fragmentShader);
    free(fshaderstr);

    printf("Using shader: %s\n",shaderFile);

    //glValidateProgram(shaderProgram);
    //glGetProgramiv(shaderProgram,GL_VALIDATE_STATUS,&res);
    //printf("res start: %u\n",res);

    //Get uniform locations
    uDeltaTimeLocation = glGetUniformLocation(shaderProgram,"DeltaTime");
    uTotalTimeLocation = glGetUniformLocation(shaderProgram,"TotalTime");
    uMousePositionLocation = glGetUniformLocation(shaderProgram,"MousePosition");
    uWindowResolutionLocation = glGetUniformLocation(shaderProgram,"Resolution");
    
    uniformWindowResolution[0] = GetWindowWidth();
    uniformWindowResolution[1] = GetWindowHeight();
}

w_LPoint mpos;

void Update()
{
    //set delta time
    uniformDeltaTime = GetDeltatime();

    //set total time
    uniformTotalTime += GetDeltatime();

    //set mouse pos
    GetMousePosition(&mpos);
    uniformMousePosition[0] = mpos.x;
    uniformMousePosition[1] = GetWindowHeight() - mpos.y;
    uniformMousePosition[2] = GetKey(VK_LBUTTON);
    uniformMousePosition[3] = GetKey(VK_RBUTTON);

    if(GetKeyDown(VK_ESCAPE))
        ExitApplication();

    //this is required for opengl to render a frame
    DisplayDirect();
}

void Render()
{
    glClearColor(1, 1, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glUseProgram(shaderProgram);

    //send values
    glUniform4fv(uMousePositionLocation,1,uniformMousePosition);
    glUniform2fv(uWindowResolutionLocation,1,uniformWindowResolution);
    glUniform1f(uDeltaTimeLocation,uniformDeltaTime);
    glUniform1f(uTotalTimeLocation,uniformTotalTime);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, &sqvert[0]);
    glEnableVertexAttribArray(0);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

int main(int argc, char** argv)
{
    int width = 1000, height = 1000;
    if(argc == 1)
    {
        if(FileExists("shader1.shader"))
        {
            shaderFile = "shader1.shader";
        }
        else
        {
            printf("No input shader. Press enter to exit...");
            char c = getchar();
            return 0;
        }
    }
    else
    {
        shaderFile = argv[1];
    }

    for(int i = 1; i < argc; i++)
    {
        if((!strcmp(argv[i],"-w") || !strcmp(argv[i],"--width")) && i+1 < argc)
        {
            long w = 0;
            if(_StringToLongB(argv[i+1],&w,F_NONEGATIVE | F_NOZERO,10))
            {
                width = w;
            }
        }
        else if((!strcmp(argv[i],"-h") || !strcmp(argv[i],"--height")) && i+1 < argc)
        {
            long h = 0;
            if(_StringToLongB(argv[i+1],&h,F_NONEGATIVE | F_NOZERO,10))
            {
                height = h;
            }
        }
    }


    w_ApplicationInit(argc,argv,width,height,"Shaderthing");
    SetRenderMode(W_RENDERER_GL);
    w_SetFPSLimit(100);
    w_RegisterRender(Render);
    w_RegisterUpdate(Update);
    w_RegisterStart(Start);
    w_ApplicationStart();
}