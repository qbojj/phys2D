#include "GL3_GuiRenderer.h"

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

void APIENTRY deb_callback(
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

GL3_GuiRenderer::GL3_GuiRenderer()
{
    if( !glfwInit() ) throw std::runtime_error("glfw could not be initialized");

    if( !ImGui::CreateContext() )
    {
        glfwTerminate();
        throw std::runtime_error("imgui could not be initialized");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_CONTEXT_DEBUG, GLFW_TRUE);

    glfwWindowHint(GLFW_SAMPLES, 16);

    window = glfwCreateWindow( 1024, 768, "phys2D", nullptr, nullptr );
    if( !window )
    {
        glfwTerminate();
        ImGui::DestroyContext();
        throw std::runtime_error("could not create window");
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

    camZoom = 10.f;
    camPos = glm::vec2( 0.f, 0.f );

    object_selected = false;
    pulling_object = false;
}

GL3_GuiRenderer::~GL3_GuiRenderer()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();

    glDeleteBuffers( 1, &buffer );
    glDeleteProgram( program );
    glDeleteVertexArrays( 1, &vao );
    
    glfwDestroyWindow( window );
    ImGui::DestroyContext();
}

bool GL3_GuiRenderer::onDraw( std::vector<PhysicsObject> &objs, double dt )
{
    glfwPollEvents();

    ImGui_ImplGlfw_NewFrame();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();

    ImGuiIO &io = ImGui::GetIO();
    const ImVec2 mouse_pos_ = ImGui::GetMousePos();
    const ImVec2 display_size = io.DisplaySize;

    const glm::dvec2 rel_mouse_pos{ 
        (double)mouse_pos_.x - display_size.x / 2.0, 
        -( (double)mouse_pos_.y - display_size.y / 2.0 ) };

    const glm::dvec2 word_mouse_pos = (rel_mouse_pos / camZoom) + camPos;

    ImGui::ShowMetricsWindow();

    if( ImGui::Begin("main stat window") )
    {
        ImGui::Text("Camera position: %g %g", camPos.x, camPos.y );
        ImGui::Text("Camera zoom (meters per pixel): %g", camZoom );
        ImGui::Checkbox("fill objects", &bTriangles );
    }
    ImGui::End();

    if( object_selected )
    {
        if( ImGui::Begin("PhysicalObject info", &object_selected) )
        {
            PhysicsObject &obj = objs[object_idx];

            ImGui::Value("index", object_idx);
            ImGui::Value("mass", (float)obj.mass);
            ImGui::Value("moment of inertia", (float)obj.moment_of_intertia);
            ImGui::Text("center: %g, %g", obj.center.x, obj.center.y );
            ImGui::Text("velocity: %g, %g", obj.velocity.x, obj.velocity.y );
            ImGui::Text("angular velocity: %g", obj.ang_velocity );
            ImGui::Text("flags: 0x%x", obj.flags);
        }
        ImGui::End();
    }

    if( !io.WantCaptureMouse )
    {
        // ImGui didn't capture mouse input process it by ourselves

        if( io.MouseClicked[ImGuiMouseButton_Left] )
        {
            bool object_found = false;

            for( size_t i = 0; i < objs.size(); i++ )
                if( is_point_inside_object( objs[i], word_mouse_pos ) )
                {
                    object_selected = true;
                    object_idx = (uint32_t)i;
                    object_found = true;
                    
                    pulling_object = true;
                    pull_pos = word_mouse_pos - objs[i].center;
                    pull_last_angle = objs[i].angle;

                    object_found = true;
                    break;
                }

            if( !object_found )
            {
                object_selected = false;
                pulling_object = false;
            }
        }
    }

    for( PhysicsObject &obj : objs )
        obj.add_impulse( {0, 0}, { 0.0, -9.81 * dt } );
    
    if( object_selected && pulling_object )
    {
        if( !io.MouseDown[ImGuiMouseButton_Left] )
            pulling_object = false;
        else
        {
            // calculate pull force
            PhysicsObject &obj = objs[object_idx];

            const double d_angle = obj.angle - pull_last_angle;
            
            const double c_a = cos( d_angle ),
                         s_a = sin( d_angle );

            const glm::mat2 rot_d_angle{
                c_a, -s_a,
                s_a,  c_a
            };

            pull_pos = rot_d_angle * pull_pos;
            pull_last_angle = obj.angle;

            const double pull_factor = 0.1;

            const glm::dvec2 pull_glob_pos = pull_pos + obj.center;

            pullArrowDir = word_mouse_pos - pull_glob_pos;

            const glm::dvec2 pull_force = pullArrowDir * pull_factor;

            obj.add_impulse( pull_pos, pull_force * dt );

            pullArrowLen = glm::length( pullArrowDir ) * camZoom * 0.02;
        }
    }
    
    renderScene( objs );

    return !glfwWindowShouldClose( window );
}

const glm::u8vec4 white( 255, 255, 255, 255 ),
                  red( 255, 0, 0, 255 );

std::vector<GL3_GuiRenderer::VertexInput>
GL3_GuiRenderer::prepareVerts( std::vector<PhysicsObject> &objs )
{
    std::vector<VertexInput> verts;

    for( size_t obj_i = 0; obj_i < objs.size(); obj_i++ )
    {
        PhysicsObject &obj = objs[obj_i];

        glm::dmat4 transorm_to_global = 
            glm::translate(
                glm::rotate( glm::dmat4( 1.0 ),
                    obj.angle, glm::dvec3(0, 0, -1) ),
                glm::dvec3(obj.center, 0)
            );
        
        for( size_t i = 0; i < obj.points.size(); i++ )
            verts.push_back( {
                glm::vec2( transorm_to_global * glm::dvec4( obj.points[i], 0, 1 ) ),
                (object_selected && obj_i == object_idx) ? red : white
            } );
    }

    return verts;
}

std::vector<uint32_t> GL3_GuiRenderer::renderTriangles( std::vector<PhysicsObject> &objs )
{
    std::vector<uint32_t> indeces;

    uint32_t offset = 0;

    for( const PhysicsObject &obj : objs )
    {
        uint32_t point_cnt = (uint32_t)obj.points.size();
        // create triangle strip from the list
        uint32_t front_it = offset + 1, 
                 back_it = offset + point_cnt - 1;

        indeces.push_back( offset );
        
        while( front_it != back_it )
        {
            indeces.push_back( front_it++ );
            if( front_it == back_it ) break;
            indeces.push_back( back_it-- );
        }

        indeces.push_back( front_it );
        indeces.push_back( INT32_MAX ); // primitive restart index

        offset += point_cnt;
    }

    return indeces;
}

std::vector<uint32_t> GL3_GuiRenderer::renderLines( std::vector<PhysicsObject> &objs )
{
    std::vector<uint32_t> indeces;

    uint32_t offset = 0;

    for( const PhysicsObject &obj : objs )
    {
        // create triangle strip from the list
        for( uint32_t i = 0; i < obj.points.size(); i++ )
            indeces.push_back( i + offset );

        indeces.push_back( offset );
        indeces.push_back( INT32_MAX ); // primitive restart index

        offset += (uint32_t)obj.points.size();
    }

    return indeces;
}

void GL3_GuiRenderer::renderScene( std::vector<PhysicsObject> &objs )
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
    ) * glm::scale(
            glm::translate(
                glm::mat4( 1.f ), 
                glm::vec3( glm::vec2(camPos), 0.f ) ),
            glm::vec3( camZoom ) );

    glUniformMatrix4fv( mViewLocation, 1, GL_FALSE, glm::value_ptr( global_view ) );
    
    auto verts = prepareVerts( objs );
    auto indeces = bTriangles ? renderTriangles( objs ) : renderLines( objs );

    size_t object_vert_count = verts.size();

    if( object_selected && pulling_object )
    {
        // add an arrow representing pulling force

        const float a1 = (float)pullArrowLen * 0.03,
                     h1 = (float)pullArrowLen * 0.8,
                     a2 = (float)pullArrowLen * 0.2;
        
        const glm::vec2 arrow[]{
            {a1, 0},
            {a1, h1},
            {-a1, 0},
            {-a1, 0},
            {a1, h1},
            {-a1, h1},
            {a2, h1},
            {0, pullArrowLen},
            {-a2, h1}
        };
        
        const glm::vec3 arrow_start( objs[object_idx].center + pull_pos, 0.0f );

        const glm::mat4 transform_arrow = glm::rotate( 
                glm::translate(
                    glm::mat4( 1.f ),
                    arrow_start ),
                atan2f( pullArrowDir.y, pullArrowDir.x ) - M_PI_2f,
                glm::vec3( 0.f, 0.f, 1.f )
            );

        for( const glm::vec2 &pos : arrow )
            verts.push_back( { 
                transform_arrow * glm::vec4( pos, 0.0f, 1.0f ), 
                white
            } );
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);

    ssize_t verts_size = verts.size() * sizeof(verts[0]);
    ssize_t indeces_size = indeces.size() * sizeof(indeces[0]);

    glBufferData(GL_ARRAY_BUFFER, verts_size + indeces_size, NULL, GL_STREAM_DRAW);
    
    glBufferSubData(GL_ARRAY_BUFFER, 0, verts_size, verts.data() );
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, verts_size, indeces_size, indeces.data() );

    glEnable(GL_PRIMITIVE_RESTART);
    glPrimitiveRestartIndex( INT32_MAX );

    glDrawElements(bTriangles ? GL_TRIANGLE_STRIP : GL_LINE_STRIP, 
        indeces_size, GL_UNSIGNED_INT, (void *)verts_size );
    
    if( object_selected && pulling_object )
        glDrawArrays(GL_TRIANGLES, object_vert_count, 9);

    ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData() );
    glfwSwapBuffers( window );
}