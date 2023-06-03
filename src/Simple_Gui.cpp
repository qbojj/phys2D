/*
Jakub Janeczko
klasa GUI
3.06.2023
*/

#include "Simple_Gui.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>

#include <stdexcept>

const glm::u8vec4 white( 255, 255, 255, 255 ),
                  red( 255, 0, 0, 255 );

Simple_Gui::Simple_Gui( Simple_PhysicsEngine *engine )
    : engine(engine)
{
    if( !engine ) 
        throw std::invalid_argument(u8"Simple_Gui musi otrzymać Simple_PhysicsEngine");

    camPos = glm::dvec2( 0.0 );
    camZoom = 10.;

    bTriangles = false;
    object_selected = false;
    creation_mode = false;
    error_popup = false;
    pulling_scene = false;
}

static void imgui_double_slider( const char *label, double &val, double min, double max )
{
    ImGui::SliderScalar(label, ImGuiDataType_Double, &val, &min, &max );
}

renderer_info
Simple_Gui::handle_gui( std::vector<PhysicsObject> &objs, double dt )
{
    renderer_info ri;

    ImGuiIO &io = ImGui::GetIO();
    const ImVec2 m_pos = ImGui::GetMousePos();
    const ImVec2 d_size = ImGui::GetMainViewport()->Size;

    const glm::dvec2 rel_mouse_pos{ 
           (double)m_pos.x - d_size.x / 2.0, 
        -( (double)m_pos.y - d_size.y / 2.0 ) };

    const glm::dvec2 world_mouse_pos = (rel_mouse_pos / camZoom) + camPos;

    ImGui::ShowMetricsWindow();

    if( ImGui::Begin(u8"Główne okno") )
    {
        ImGui::Text(u8"Pozycja kamery: x=%g, y=%g", camPos.x, camPos.y );
        ImGui::Text(u8"Przybliżenie: %g px/m", camZoom );
        ImGui::Checkbox(u8"Wypełniać obiekty?", &bTriangles );

        bool last_creation_mode = creation_mode;
        ImGui::Checkbox(u8"Menu tworzenia obiektów", &creation_mode );
        if( !last_creation_mode && creation_mode )
        {
            density = 1.0;
            point_cloud.clear();
            flags_created_item = 0;
        }
    }
    ImGui::End();

    if( ImGui::Begin( u8"Paramety silnika fizyki") )
    {
        imgui_double_slider(u8"Siła grawitacji [m/s2]", engine->gravity, 0, 100 );
        imgui_double_slider(u8"Składowa pomniejszania prędkości", 
            engine->dump_velocity_factor, 0, 0.1);
        imgui_double_slider(u8"Składowa pomniejszania prędkości kątowej",
            engine->dump_angular_velocity_factor, 0, 0.1 );
        imgui_double_slider(u8"Spręrzystość odbicia (ile energi zostaje przy odbiciu)",
            engine->restitution, 0, 1 );
        
        ImGui::SliderInt(u8"Liczba podkroków symulacji",
            &engine->time_subdivision, 1, 1024 );
    }
    ImGui::End();

    if( object_selected )
    {
        if( ImGui::Begin(u8"Parametry symulowanego obiektu", &object_selected) )
        {
            PhysicsObject &obj = objs[object_idx];

            ImGui::Value("Index", object_idx);
            ImGui::Value(u8"Odwrotność masy [1/kg]", (float)obj.inv_mass);
            ImGui::Value(u8"Odwrotność momentu bezwładności [1/(kg*m2)]", 
                (float)obj.inv_moment_of_intertia);

            ImGui::Text(u8"Środek [m]: x=%g, y=%g", obj.center.x, obj.center.y );
            ImGui::Text(u8"Kąt [rad]: %g", obj.angle);
            ImGui::Text(u8"Prędkość [m/s]: x=%g, y=%g", obj.velocity.x, obj.velocity.y );
            ImGui::Text(u8"Prędkość kątowa [rad/s]: %g", obj.ang_velocity );
            ImGui::Text(u8"Flagi: %s", 
                obj.flags & PhysicsObject::Immovable ? u8"nieruszalny" : u8"");
        }
        ImGui::End();
    }

    if( creation_mode )
    {
        for( const glm::dvec2 &pt : point_cloud )
            ri.additional_points.push_back( pt );
        
        try
        {
            PhysicsObject new_obj( point_cloud, density, flags_created_item );

            ri.additional_lines.push_back( new_obj.points[0] + new_obj.center );
            for( size_t i = 1; i < new_obj.points.size(); i++ )
            {
                const glm::dvec2 &pt = new_obj.points[i];
                ri.additional_lines.push_back( pt + new_obj.center );
                ri.additional_lines.push_back( pt + new_obj.center );
            }
            ri.additional_lines.push_back( new_obj.points[0] + new_obj.center );
        }
        catch(const std::exception& e)
        {
            if( point_cloud.size() == 2 )
            {
                ri.additional_lines.push_back( point_cloud[0] );
                ri.additional_lines.push_back( point_cloud[1] );
            }
        }

        if( ImGui::Begin(u8"Menu tworzenia", &creation_mode) )
        {
            imgui_double_slider(u8"Gęstość", density, 0.01, 10);
            ImGui::CheckboxFlags(u8"Nieruszalny?", 
                &flags_created_item, PhysicsObject::Immovable );
            
            if( ImGui::Button(u8"Utwórz") )
            {
                try
                {
                    objs.emplace_back( point_cloud, density, flags_created_item );
                    point_cloud.clear();
                }
                catch(const std::exception& e)
                {
                    error_popup = true;
                    error_message = e.what();
                }
            }

            ImGui::SameLine();
            
            if( ImGui::Button(u8"Wyczyść punktu") )
                point_cloud.clear();
        }
        ImGui::End();
    }

    if( error_popup )
    {
        if( ImGui::Begin(u8"Błąd", &error_popup) )
            ImGui::Text(u8"Błąd: %s", error_message.c_str());
        
        ImGui::End();
    }

    if( !io.WantCaptureMouse )
    {
        // ImGui didn't capture mouse input process it by ourselves

        if( creation_mode )
        {
            if( ImGui::IsMouseClicked(ImGuiMouseButton_Left) )
                point_cloud.push_back( world_mouse_pos );
        }
        else if( ImGui::IsMouseClicked(ImGuiMouseButton_Left) )
        {
            bool object_found = false;

            for( size_t i = 0; i < objs.size(); i++ )
                if( objs[i].is_point_inside_object( world_mouse_pos ) )
                {
                    object_selected = true;
                    object_idx = (uint32_t)i;
                    object_found = true;
                    
                    pulling_object = true;
                    pull_pos = world_mouse_pos - objs[i].center;
                    pull_last_angle = objs[i].angle;

                    object_found = true;
                    break;
                }

            if( !object_found )
            {
                object_selected = false;
                pulling_object = false;
                pulling_scene = true;
            }
        }

        if( pulling_scene || ImGui::IsMouseDown(ImGuiMouseButton_Right) )
        {
            if( !ImGui::IsMouseDown(ImGuiMouseButton_Left) )
                pulling_scene = false;
            
            const glm::dvec2 mouse_delta{ io.MouseDelta.x, -io.MouseDelta.y };
            camPos -= mouse_delta / camZoom;
        }

        camZoom *= pow( 1.1, io.MouseWheel );
    }

    if( !io.WantCaptureKeyboard )
    {
        if( ImGui::IsKeyDown(ImGuiKey_Delete) )
        {
            if( object_selected )
            {
                objs.erase(objs.begin() + object_idx);
                object_selected = false;
                pulling_object = false;
            }
        }
    }
    
    if( object_selected && pulling_object )
    {
        if( !ImGui::IsMouseDown(ImGuiMouseButton_Left) )
            pulling_object = false;
        else
        {
            // calculate pull force
            PhysicsObject &obj = objs[object_idx];

            const double d_angle = obj.angle - pull_last_angle;
            
            const double c_a = cos( d_angle ),
                         s_a = sin( d_angle );

            const glm::mat2 rot_d_angle{
                 c_a, s_a,
                -s_a, c_a
            };

            pull_pos = rot_d_angle * pull_pos;
            pull_last_angle = obj.angle;

            const double pull_factor = 100;

            const glm::dvec2 pull_glob_pos = pull_pos + obj.center;

            const glm::dvec2 pullArrowDir = world_mouse_pos - pull_glob_pos;

            const glm::dvec2 pull_force = pullArrowDir * pull_factor;

            obj.add_impulse( pull_pos, pull_force * dt );

            const double pullArrowLen = glm::length( pullArrowDir ) * 0.2;

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
            
            const glm::vec3 arrow_start( obj.center + pull_pos, 0.0f );

            const glm::mat4 transform_arrow = glm::rotate( 
                    glm::translate(
                        glm::mat4( 1.f ),
                        arrow_start ),
                    atan2f( pullArrowDir.y, pullArrowDir.x ) - M_PI_2f,
                    glm::vec3( 0.f, 0.f, 1.f )
                );

            for( const glm::vec2 &pos : arrow )
                ri.additional_triangles.push_back(
                    transform_arrow * glm::vec4( pos, 0.0f, 1.0f )
                );
        }
    }

    ri.camPos = camPos;
    ri.camZoom = camZoom;
    ri.fill_objects = bTriangles;

    for( size_t i = 0; i < objs.size(); i++ )
        ri.object_colors.push_back(
            object_selected && object_idx == i ?
                red : white
        );

    return ri; 
}