/*
Jakub Janeczko
Renderer, czyli wyświetlanie sceny i gui
06.06.2023
*/

#include "GL3_Renderer.h"

#define GLFW_INCLUDE_NONE
#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stdexcept>
#include <string>

static const char *vert_code = R"(
#version 330

uniform mat4 mView;
layout(location = 0) in vec2 vPos;
layout(location = 1) in vec4 vColor;

out vec4 frag_vColor;

void main()
{
    gl_Position = mView * vec4( vPos, 0, 1 );
    frag_vColor = vColor;
}
)";

static const char *frag_code = R"(
#version 330

out vec4 col;
in vec4 frag_vColor;

void main()
{
    col = frag_vColor;
}
)";

static void APIENTRY deb_callback(
    GLenum, //source,
    GLenum type,
    GLuint, //id,
    GLenum severity,
    GLsizei, //length,
    const GLchar *message,
    const void *) //userParam )
{
    fprintf( stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
           ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),
            type, severity, message );
}

GL3_Renderer::GL3_Renderer()
{
    if( !glfwInit() ) throw std::runtime_error(u8"glfw nie mogło się zainicjalizować");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);

    glfwWindowHint(GLFW_SAMPLES, 16);

    window = glfwCreateWindow( 1024, 768, u8"phys2D", nullptr, nullptr );
    if( !window )
    {
        glfwTerminate();

        const char *msg;
        glfwGetError( &msg );
        throw std::runtime_error(u8"nie można otworzyć okna: " + std::string(msg));
    }

    glfwMakeContextCurrent( window );
    gladLoadGL( glfwGetProcAddress );
    glfwSwapInterval(0);

    if( GLAD_GL_VERSION_4_3 )
    {
        // enable debug output
        glEnable( GL_DEBUG_OUTPUT );
        glDebugMessageCallback( deb_callback, nullptr );
    }

    ImGuiIO &io = ImGui::GetIO();

    ImFontGlyphRangesBuilder builder;
    builder.AddRanges(io.Fonts->GetGlyphRangesDefault());
    builder.AddText(u8"ąĄęĘżŻźŹłŁćĆóÓśŚńŃ");

    ImVector<ImWchar> ranges;
    builder.BuildRanges( &ranges );

    io.Fonts->AddFontFromFileTTF("times.ttf", 14, nullptr, ranges.Data );
    io.Fonts->Build();

    ImGui_ImplGlfw_InitForOpenGL( window, true );
    ImGui_ImplOpenGL3_Init();

    GLuint vert = glCreateShader( GL_VERTEX_SHADER );
    GLuint frag = glCreateShader( GL_FRAGMENT_SHADER );

    glShaderSource( vert, 1, &vert_code, nullptr );
    glShaderSource( frag, 1, &frag_code, nullptr );

    glCompileShader( vert );
    glCompileShader( frag );

    program = glCreateProgram();
    glAttachShader( program, vert );
    glAttachShader( program, frag );
    glLinkProgram( program );

    mViewLocation = glGetUniformLocation( program, "mView" );

    glDeleteShader( vert );
    glDeleteShader( frag );

    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(VertexInput), 0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, 
        sizeof(VertexInput), (void *)offsetof( VertexInput, vColor ) );

    glEnable(GL_MULTISAMPLE);

    glfwPollEvents();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

GL3_Renderer::~GL3_Renderer()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();

    glDeleteBuffers( 1, &buffer );
    glDeleteProgram( program );
    glDeleteVertexArrays( 1, &vao );
    
    glfwDestroyWindow( window );
}

std::vector<GL3_Renderer::VertexInput>
GL3_Renderer::prepareVerts( const renderer_info &ri, const std::vector<PhysicsObject> &objs )
{
    std::vector<VertexInput> verts;

    for( size_t obj_i = 0; obj_i < objs.size(); obj_i++ )
    {
        const PhysicsObject &obj = objs[obj_i];
        
        for( size_t i = 0; i < obj.points.size(); i++ )
            verts.push_back( {
                glm::vec2( obj.points[i] + obj.center ),
                ri.object_colors[obj_i]
            } );
    }

    return verts;
}

std::vector<uint32_t> GL3_Renderer::renderTriangles( const std::vector<PhysicsObject> &objs )
{
    std::vector<uint32_t> indices;

    uint32_t offset = 0;

    for( const PhysicsObject &obj : objs )
    {
        uint32_t point_cnt = (uint32_t)obj.points.size();
        
        // utwórz 'triangle strip' z listy punktów
        uint32_t front_it = offset + 1, 
                 back_it = offset + point_cnt - 1;

        indices.push_back( offset );
        
        while( front_it != back_it )
        {
            indices.push_back( front_it++ );
            if( front_it == back_it ) break;
            indices.push_back( back_it-- );
        }

        indices.push_back( front_it );
        indices.push_back( INT32_MAX ); // primitive restart index

        offset += point_cnt;
    }

    return indices;
}

std::vector<uint32_t> GL3_Renderer::renderLines( const std::vector<PhysicsObject> &objs )
{
    std::vector<uint32_t> indices;

    uint32_t offset = 0;

    for( const PhysicsObject &obj : objs )
    {
        // utwórz 'line strip' z listy punktów
        for( uint32_t i = 0; i < obj.points.size(); i++ )
            indices.push_back( i + offset );

        indices.push_back( offset );
        indices.push_back( INT32_MAX ); // primitive restart index

        offset += (uint32_t)obj.points.size();
    }

    return indices;
}

bool GL3_Renderer::draw( const renderer_info &ri,
    const std::vector<PhysicsObject> &objs, double )
{
    ImGui::Render();

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    int width, height;
    glfwGetFramebufferSize( window, &width, &height );

    glViewport( 0, 0, width, height );
    glScissor( 0, 0, width, height );

    float width_half_f = width * 0.5f,
          height_half_f = height * 0.5f;
    
    glUseProgram(program);
    glBindVertexArray(vao);
    
    const glm::mat4 global_view = glm::ortho(
        -width_half_f, width_half_f,
        -height_half_f, height_half_f
    ) * glm::translate(
            glm::scale(
                glm::mat4( 1.f ), 
                glm::vec3( (float)ri.camZoom ) ),
            glm::vec3( -glm::vec2( ri.camPos ), 0.f ) );

    glUniformMatrix4fv( mViewLocation, 1, GL_FALSE, glm::value_ptr( global_view ) );
    
    auto verts = prepareVerts( ri, objs );
    auto indices = ri.fill_objects ? renderTriangles( objs ) : renderLines( objs );

    size_t object_vert_count = verts.size();

    for( const glm::dvec2 &pos : ri.additional_points )
        verts.push_back( { glm::vec2(pos), glm::u8vec4(255,255,255,255) } );

    for( const glm::dvec2 &pos : ri.additional_lines )
        verts.push_back( { glm::vec2(pos), glm::u8vec4(255,255,255,255) } );

    for( const glm::dvec2 &pos : ri.additional_triangles )
        verts.push_back( { glm::vec2(pos), glm::u8vec4(255,255,255,255) } );
    
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);

    size_t verts_size = verts.size() * sizeof(verts[0]);
    size_t indices_size = indices.size() * sizeof(indices[0]);

    glBufferData(GL_ARRAY_BUFFER, verts_size + indices_size, NULL, GL_STREAM_DRAW);
    
    glBufferSubData(GL_ARRAY_BUFFER, 0, verts_size, verts.data() );
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, verts_size, indices_size, indices.data() );

    glEnable(GL_PRIMITIVE_RESTART);
    glPrimitiveRestartIndex( INT32_MAX );

    glDrawElements(ri.fill_objects ? GL_TRIANGLE_STRIP : GL_LINE_STRIP, 
        (GLsizei)indices_size, GL_UNSIGNED_INT, (void *)verts_size );
    
    glPointSize(3);
    size_t vert_offset = object_vert_count;
    glDrawArrays(GL_POINTS, (GLint)vert_offset, (GLsizei)ri.additional_points.size());
    
    vert_offset += ri.additional_points.size();
    glDrawArrays(GL_LINES, (GLint)vert_offset, (GLsizei)ri.additional_lines.size() );

    vert_offset += ri.additional_lines.size();
    glDrawArrays(GL_TRIANGLES, (GLint)vert_offset, (GLsizei)ri.additional_triangles.size());

    ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData() );
    glfwSwapBuffers( window );

    glfwPollEvents();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    return !glfwWindowShouldClose(window);
}