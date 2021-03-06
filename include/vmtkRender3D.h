#ifndef VMTKRENDER3D_H
#define VMTKRENDER3D_H

#include "vmtkFrameBufferObject.h"
#include "importDicom.h"
#include "shaders.h"
#include "vmath.h"
#include "transferFunction.h"
#include "equalize.h"

#include<fstream>
#include <iostream>
#include <string>
#include <sstream>

template <typename T>
std::string NumberToString ( T Number )
{
    std::ostringstream ss;
    ss << Number;
    return ss.str();
}
#define STRCAT(A, B) A B
class vmtkRender3D
{
	public:

	    /**
		* @brief vmtkRenderer3D constructor
		*/
		vmtkRender3D();

        /**
         * @brief sets the volumes to be registered.
         * @param[in] acqVector: vector of the volumes.
         */
        void setAcquisition(std::vector<Import::ImgFormat*> acqVector);

        /**
         * @brief get acquisition of the volumes.
         * @param [in] id: vector index of the volume.
         * @return volume informations for index selected.
         */
        Import::ImgFormat *getAcquisition(int id);

		/**
        * @brief initializes 3D volume rendering.
		*/
		void initialize(int width, int height);
		/**
		* @brief renders the data with the specified redering mode.
		*/
		void render();
		/**
		* @brief renders the color cube in the fbos.
        * @param[in] mvp: modelview matrix.
		*/
		void preRender(vmath::Matrix4f mvp);
		/**
         * @brief resizes the dimensions of the display.
         * @param[in] w: width of image.
         * @param[in] h: height of image.
		 */
		void resize(int width, int height);
		/**
		* @brief sets the rotation matrix.
        * @param[in] rotation matrix.
		*/
        void setRotation(float ax, float ay, float az);
		/**
		* @brief sets the threshold.
        * @param[in]  threshold.
		*/
        void setThreshold(int threshold);
		/**
		* @brief sets the blender factor.
        * @param [in] blender factor.
		*/
        void setBlender(float blender);
		/**
		* @brief draws a unit cube to get the front and the back depth maps of the volume cube,
		* after updating the clipping plane values.
        * @param [in] mvp: projection*modelview matrix.
		*/
        void itlDrawColorCube(vmath::Matrix4f mvp);
		
		/**
		* @brief initializes the geometry of a cube to be rendered for getting back and front depth maps.
		*/
		void initDrawCube();

        /**
         * @brief reads the co-register matrix.
         * @param [in] s: co-register matrix file name.
         * @param [out] invRM: inverse matrix.
         * @return
         */
        bool readMatrix(const char *s, vmath::Matrix4f& invRM);

        /**
         * @brief readPlane
         * @param [in] s: string file name.
         * @param [out] eqp: Equation of the plane for multiplanar reformation.
         * @return
         */
        bool readPlane(const char *s, vmath::Vector4f &eqp);
		
		/**
		* @brief sets the clipping plane.
		* @param[in] slice
		*/
		void setClipLeftX(float left_x);
		
		/**
		* @brief gets the maximum number of the slices from the axis X.
		*/
		int getMaxSliceLeft();

        /**
         * @brief volumeRealDimension
         * @param [in] data: volume information's.
         * @param [out] vrd: real Dimension of the volume.
         */
        void volumeRealDimension(Import::ImgFormat *data, float vrd[]);

        /**
         * @brief maximumDimension
         * @param vrd (real Dimension of the volume).
         * @return Maximum dimension of the volume.
         */
        float maximumDimension(float vrd[]);

        /**
         * @brief scaleFactors
         * @param [in] vrd: eal Dimension of the volume.
         * @param [in] maxDim: Maximum dimension of the volume.
         * @return scale factor of the volume.
         */
        vmath::Vector4f scaleFactors(float vrd[], float maxDim);

        /**
         * @brief phyDimension
         * @param [in] data: volume information's.
         * @return Physical dimension.
         */
        vmath::Vector4f phyDimension(Import::ImgFormat *data);

        /**
         * @brief currentActivateTexture
         * @return number of the current texture activated.
         */
        int currentActivateTexture();

        /**
         * @brief mapEqualizeHistogramVolume
         * @param [in] data: volume information's.
         * @param [out] map: histogram equalization mapping of the volume.
         */
        void mapEqualizeHistogramVolume(Import::ImgFormat *data, unsigned short *&map);

        /**
         * @brief volumeEqualizer
         * @param [in] data: volume information's.
         * @param [in] map: histogram equalization mapping of the volume.
         * @param [out] texbuffer: buffer of the volume equalizer.
         */
        void volumeEqualizer(Import::ImgFormat *data, unsigned short *map, unsigned short *&texbuffer);

        /**
         * @brief texture3DFromVolume
         * @param [in] data: volume information's.
         * @param [in] texbuffer: buffer of the volume equalizer.
         * @param [out] idTexture: id volume texture (texture name).
         */
        void texture3DFromVolume(Import::ImgFormat *data, unsigned short *texbuffer, GLuint &idTexture);

        /**
         * @brief generateTransferFunction
         * @param [in] data: volume information's.
         * @param [in] texbuffer: buffer of the volume equalizer.
         * @param [out] tf: buffer transfer function.
         */
        void generateTransferFunction(Import::ImgFormat *data, int &dim, unsigned char *&tf);

        /**
         * @brief texture1DFromTransferFunction
         * @param [in] dim: dimension transfer fucntion mapping.
         * @param [in] tf: buffer transfer function.
         * @param [out] idTF: id transfer function texture (texture name).
         */
        void texture1DFromTransferFunction(int dim, unsigned char *tf, GLuint &idTF);

        /**
         * @brief setMPR
         * @param [in] eqp: equation plane for multiplanar reformatting.
         */
        void setMPR(vmath::Vector4f eqp);

        /**
         * @brief setEnableMPR
         * @param [in] enableMPR: enable MPR from keys interaction's, if equation file exist.
         */
        void setEnableMPR(bool enableMPR);

        /**
         * @brief setStateMPRInput
         * @param [in] stateMPRInput: file input status for MPR.
         */
        void setStateMPRInput(bool stateMPRInput);

        /**
         * @brief setVectorInvMatrixReg
         * @param [in] imr: vector of the inverse matrix register.
         */
        void setVectorInvMatrixReg(std::vector<vmath::Matrix4f> imr);

private:

        int m_maxSliceLeft;                     /**< dimension volume for X-axis */
        float m_blender;                        /**< blender for the reference and float volumes. */
        vmath::Matrix4f *m_invRegMatrix;        /**< inverse matrix array for register */
        std::vector<unsigned short *> m_map;      /**< map vector of the volumes */
        std::vector<float> m_Threshold;         /**< threshold vector for volumes */
        std::vector<Import::ImgFormat*> m_data; /**< data vector of the volumes. */
        std::vector<GLuint>m_idTexture, m_idTF; /**< id textures */
        vmath::Vector4f m_equationPlaneForMPR;  /**< equation plane for multiplanar reformatting */
        bool m_enableMPR;                       /**< enable multiplanar reformatting  */
        bool m_stateMPRInput;                   /**< state load equation for multiplanar reformatting */

	    GLuint m_ColorShader;    /**< pre-processing shader to get front and back planes */
		GLuint m_RaytraceShader; /**< raycast shader */
	
        GLuint vaosCube,            /**< drawCube: vertex array object */
            vboCube,                /**< drawCube: data buffer */
            eboCube,                /**< drawCube: element array buffer */
            vaosRenderPlane,        /**< RenderPlane: vertex array object */
            vboRenderPlane,         /**< RenderPlane: data buffer */
            eboRenderPlane,         /**< RenderPlane: element array buffer */
            vaosResultPlane,        /**< renderResult: vertex array object */
            vboResultPlane,         /**< renderResult: data buffer */
            eboResultPlane;         /**< renderResult: element array buffer */
			
		/**
		 * @struct VertexDataCube
		 */
		struct VertexDataCube
		{
			vmath::Vector3f position; /**< position coordinates */
		};

		/**
		* @struct VertexData
		*/
		struct VertexData
		{
			vmath::Vector2f position; /** < position coordinates */
			vmath::Vector2f texCoord; /** < texture coordinates */
		};
		 /**
		 * @brief Position vectors of VertexData.
		 */
		vmath::Vector2f v1, v2, v3, v4;
		/**
		 * @brief Texture vectors of VertexData.
		 */
		vmath::Vector2f vt1, vt2, vt3, vt4;
		/**
		* @brief Position vectors of VertexDataCube.
		*/
		vmath::Vector3f vc1, vc2, vc3, vc4, vc5, vc6, vc7, vc8, vc9, vc10, vc11, vc12,
                    vc13, vc14, vc15, vc16, vc17, vc18, vc19, vc20, vc21, vc22, vc23, vc24;
	
		float m_fClipXLeft;                       /**< x left clip plane */
		float m_fClipXRight;                      /**< x right clip plane */
		float m_fClipYTop;                        /**< y top clip plane */
		float m_fClipYBottom;                     /**< y bottom clip plane */
		float m_fClipZBack;                       /**< z back clip plane */
		float m_fClipZFront;                      /**< z front clip plane */
	
	    int m_iHeight;                            /**< offscreen image height */
		int m_iWidth;                             /**< offscreen image width */
	
        vmath::Matrix4f m_projectionMatrix;  /**< projection transformation matrix */
		vmath::Matrix4f m_modelViewMatrix;  /**< modelview transformation matrix */
		vmath::Matrix4f m_rotationMatrix; /**< rotationMatrix transformation matrix */
		vmath::Matrix4f m_scalationMatrix; /**< scalationMatrix transformation matrix */
		vmath::Matrix4f m_translationMatrix; /**< traslationMatrix transformation matrix */
		 
        vmath::Vector4f *m_phyDimensions;       /**< physical dimensions of volumes */
        vmath::Vector4f *m_scaleFactors;        /**< scale factor for compensating distortions on the textures volumes */
		
		vmtkFrameBufferObject *m_FrontFBO;      /**< framebuffer object for front side */
		vmtkFrameBufferObject *m_BackFBO;       /**< framebuffer object for back side */
		vmtkFrameBufferObject *m_RayTracedFBO;  /**< framebuffer object for raycasted image */
		
		bool m_bInitialized;                      /**< whether the volume renderer is initialized ? */

		/**
		* @brief defines the position and texture coordinates of the four vertices of a plane.
		*/
		void createVectorPlanes();
		
        /**
         * @brief loads volumes to textures 3D.
         */
        void loadVolumesToTextures();

        /**
         * @brief Loads transfer Functions to textures 1D.
         */
        void loadTransferFunctionsToTexture();
		
		/**
		* @brief creates vertex arrays and their buffer data.
		*/
		void createBuffers();
		
		/**
		 * @brief initializes the plane to be input into the graphics pipeline
		 */
		void initRenderPlane();

        /**
		* @brief draws a cube.
		*/
		void drawCube();

        /**
        * @brief raycasts through the color cube.
        */
        void raycastingMultiVolume();
		
		/**
		* @brief draws the plane from pre-render shader.
		*/
        void drawPlaneRayTraced();

};

#endif // VMTKRENDER3D_H
