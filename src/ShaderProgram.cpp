// Robin Heinemann Copyright (C) 2015

#include "ShaderProgram.h"

ShaderProgram::ShaderProgram(const char * vertexShaderSource, const char * fragmentShaderSource) {
	m_id = glCreateProgram();
	m_vid = m_newShader(GL_VERTEX_SHADER,vertexShaderSource);
	m_fid = m_newShader(GL_FRAGMENT_SHADER, fragmentShaderSource);   
	
	glAttachShader(m_id, m_vid);
	glAttachShader(m_id, m_fid);
	glLinkProgram(m_id);
	glValidateProgram(m_id);
}

int ShaderProgram::vertexAttribPointer(const char * name, int count, int type, bool normalized, int stride, const void * offset) {
	glUseProgram(m_id);

	int attrib = glGetAttribLocation(m_id, name); 

	glEnableVertexAttribArray(attrib);
	glVertexAttribPointer(attrib, count, type, normalized, stride, offset);

	return 0;
}

int ShaderProgram::useProgram() {
	glUseProgram(m_id);

	return 0;
}

int ShaderProgram::uniform2f(const char * name, float x, float y) {
	glUseProgram(m_id);
	
	int uniformID = glGetUniformLocation(m_id, name);

	glUniform2f(uniformID, x, y);

	return 0;
}

int ShaderProgram::uniform1i(const char * name, int x) {
	glUseProgram(m_id);

	glUniform1i(glGetUniformLocation(m_id, name), x);

	return 0;
}

int ShaderProgram::bindFragDataLocation(int target, const char *name) {
	glBindFragDataLocation(m_id, target, name);

	return 0;
}

int ShaderProgram::m_newShader(int type, const char * source) {
    int id = glCreateShader(type), tmp = 0;
	int sourceLength = strlen(source);
	
	char * buf = (char *) malloc((sourceLength + 1) * sizeof(char));
	
	strcpy(buf, source);
	
	glShaderSource(id, 1, &buf, &sourceLength);
	glCompileShader(id);
		
	glGetShaderiv(id, GL_COMPILE_STATUS, &tmp);
	
	glGetShaderInfoLog(id, 1024, &tmp, buf);

	std::cout << buf << std::endl;

	free(buf);
	
	return id;
}
