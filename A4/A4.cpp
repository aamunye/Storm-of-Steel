#include <glm/ext.hpp>

#include "A4.hpp"

#include <iostream>
using namespace std;
using namespace glm;

void A4_Render(
		// What to render
		SceneNode * root,

		// Image to write to, set to a given width and height
		Image & image,

		// Viewing parameters
		const glm::vec3 & eye,
		const glm::vec3 & view,
		const glm::vec3 & up,
		double fovy,

		// Lighting parameters
		const glm::vec3 & ambient,
		const std::list<Light *> & lights
) {

  // Fill in raytracing code here...

  std::cout << "Calling A4_Render(\n" <<
		  "\t" << *root <<
          "\t" << "Image(width:" << image.width() << ", height:" << image.height() << ")\n"
          "\t" << "eye:  " << glm::to_string(eye) << std::endl <<
		  "\t" << "view: " << glm::to_string(view) << std::endl <<
		  "\t" << "up:   " << glm::to_string(up) << std::endl <<
		  "\t" << "fovy: " << fovy << std::endl <<
          "\t" << "ambient: " << glm::to_string(ambient) << std::endl <<
		  "\t" << "lights{" << std::endl;

	for(const Light * light : lights) {
		std::cout << "\t\t" <<  *light << std::endl;
	}
	std::cout << "\t}" << std::endl;
	std:: cout <<")" << std::endl;

	size_t h = image.height();
	size_t w = image.width();

	for (uint y = 0; y < h; ++y) {
		for (uint x = 0; x < w; ++x) {
			/*
			// Red: increasing from top to bottom
			image(x, y, 0) = (double)y / h;
			// Green: increasing from left to right
			image(x, y, 1) = (double)x / w;
			// Blue: in lower-left and upper-right corners
			image(x, y, 2) = ((y < h/2 && x < w/2)
						  || (y >= h/2 && x >= w/2)) ? 1.0 : 0.0;
			*/

			image(x,y,0) = ((float)x/(float)w + (float)y/(float)h)/(float)2;
			image(x,y,1) = ((float)x/(float)w + (float)y/(float)h)/(float)2;
			image(x,y,2) = ((float)x/(float)w + (float)y/(float)h)/(float)2;
		}
	}

	for (SceneNode * child : root->children) {
		cout<<*child<<endl;
	}

	uint nx = image.width();
	uint ny = image.height();

	float d1 = 200;

	float h1 = 2*d1*tan((float)fovy*(M_PI/(float)180)/2);
	//float h = 2*d*tan()
	float w1 = (float)nx/(float)ny*(float)h1;

	cout<<"nx "<<nx<<endl;
	cout<<"ny "<<ny<<endl;
	cout<<"d1 "<<d1<<endl;
	cout<<"h1 "<<h1<<endl;
	cout<<"w1 "<<w1<<endl;

	vec3 lookFrom  = eye;
	vec3 lookAt = lookFrom + view;

	// looking at point 0,0 on the screen
	glm::vec3 vpoint = vec3(0.0f, 0.0f, 0.0f);

	vec3 v_u,v_v,v_w;
	mat3 R;
	mat4 T;

	vec3 pworld;

	for (uint y = 0; y < h; ++y) {
		for (uint x = 0; x < w; ++x) {
			image(x,y,0) = 0.0f;
			image(x,y,1) = 0.0f;
			image(x,y,2) = 0.0f;
			vpoint = vec3(x,y, 0.0f);
			//vpoint = vec3(w/2,h/2, 0.0f);

			vpoint.x += (-1*(float)nx/(float)2);
			vpoint.y += (-1*(float)ny/(float)2);
			vpoint.z += (float)d1;

			vpoint.x *= (-1*(float)w1/(float)nx);
			vpoint.y *= (1*(float)h1/(float)ny);


			v_w = (lookAt - lookFrom)/length(lookAt-lookFrom);
			//bool b = ((lookAt - lookFrom)/length(lookAt-lookFrom))==((lookAt - lookFrom)/distance(lookAt,lookFrom));
			v_u = cross(up,v_w)/length(cross(up,v_w));
			v_v = cross(v_w,v_u);
			R = mat3(v_u,v_v,v_w);
			T = translate(mat4(1.0),vec3(lookFrom.x,lookFrom.y,lookFrom.z));

			vpoint = R*vpoint;

			vpoint.x += lookFrom.x;
			vpoint.y += lookFrom.y;
			vpoint.z += lookFrom.z;

			pworld = vpoint;

			//cout<<"before"<<endl;
			std::list<SceneNode*> chilun = root->children;
			float shortestT = 1000;
			for(SceneNode * child : root->children) {

				if( child->m_nodeType == NodeType::GeometryNode ) {
					GeometryNode * childGeom = static_cast<GeometryNode *>(child);
					Primitive * prim = childGeom->m_primitive;

					NonhierSphere * nhSphere = static_cast<NonhierSphere *>(prim);
					if(nhSphere != NULL){
						bool b = nhSphere->hit(lookFrom, pworld, shortestT);
					}




				}
			}

			//cout<<"x "<<x<<" y "<<y<<" "<<shortestT<<endl;
			if(shortestT < 1000) {
				image(x,y,0) = 1.0f;
			}

			//cout<<"end"<<endl;

		}
	}


}
