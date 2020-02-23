

GLuint LoadShader(GLenum shaderType, const char *shaderString)
{
    GLuint _shader;
    
    //Create the shader object
    _shader = glCreateShader(shaderType);

    if (_shader == 0)
        return 0;

    //Load the shader
    glShaderSource(_shader, 1, &shaderString, 0);
    
    //Compile the shader
    glCompileShader(_shader);

    GLint compiled;

    //Check that shader compiled properly
    glGetShaderiv(_shader, GL_COMPILE_STATUS, &compiled);

    if (!compiled) 
    {
        GLint infoLen = 0;

        glGetShaderiv(_shader, GL_INFO_LOG_LENGTH, &infoLen);
        
        if(infoLen > 1)
        {
            char* infoLog = (char*)malloc(sizeof(char) * infoLen);

            glGetShaderInfoLog(_shader, infoLen, 0, infoLog);
            printf("Failed to compile shader:\n%s", infoLog);           
            
            free(infoLog);
        }

        glDeleteShader(_shader);
        return 0;
    }

    return _shader;
}

GLuint ConstructShader(const char* vertexShader, const char* fragmentShader, const char** attributes, int num_attributes)
{
    GLuint vShader;
    GLuint fShader;
    GLuint programObject;

    //Load the vertex and fragment shaders
    vShader = LoadShader(GL_VERTEX_SHADER, vertexShader);
    fShader = LoadShader(GL_FRAGMENT_SHADER, fragmentShader);

    //Create the program object
    programObject = glCreateProgram();

    //Check that it was craeted properly
    if(programObject == 0) return 0;

    //Attatch vertex and fragment shaders
    glAttachShader(programObject, vShader);
    glAttachShader(programObject, fShader);

    //Bind attributes  
    for (size_t i = 0; i < num_attributes; ++i)
        glBindAttribLocation(programObject, i, attributes[i]);
    

    //Link the program
    glLinkProgram(programObject);

    GLint linked;

    //Check if program linked properly
    glGetProgramiv(programObject, GL_LINK_STATUS, &linked);

    if(!linked) 
    {
        GLint infoLen = 0;

        glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &infoLen);
        
        if(infoLen > 1)
        {
            char* infoLog = (char*)malloc(sizeof(char) * infoLen);

            glGetProgramInfoLog(programObject, infoLen, 0, infoLog);
            printf("Failed to link shaderprogram:\n%s", infoLog);          
            
            free(infoLog);
        }

        glDeleteProgram(programObject);
        return 0;
    }

    //Delete shaders since they are not needed anymore
    glDeleteShader(vShader);
    glDeleteShader(fShader);

    return programObject;
}