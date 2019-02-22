#pragma once
#include <d3d11.h>
#include <vector>
#include "MeshDataStructs.h"
#include "Math.h"
#include "lightclass.h"
#include "ShaderLight.h"


namespace Procedural 
{

	class CACell
	{
	public:
		CACell() {}
		CACell(Vert3D vert, bool doa) : vertex(vert), deadOrAlive(doa) {}

		Vert3D vertex;
		bool deadOrAlive;
	};



	class Terrain
	{

	private:

		std::vector<Vert3D> vertices;
		std::vector<unsigned int> indices;
		std::vector<std::vector<Vert3D>> verticesDoubleVector;
		unsigned int _numRows, _numColumns;
		float xScale = 1.0f, yScale = 1.0f, zScale = 1.0f;
		ID3D11Buffer *_vertexBuffer, *_indexBuffer;

		ID3D11ShaderResourceView* unbinder[1] = { nullptr };

		///helper functions
		inline unsigned int wr(int row);
		inline unsigned int wc(int col);
		float sampleDiamond(int i, int j, int reach);

	public:
		
		Terrain(unsigned int x = 1, unsigned int y = 1, SVec3 scales = SVec3(1, 1, 1));
		~Terrain();

		void setScales(float x, float y, float z);


		///generation methods

		//random
		void GenRandom(float chance);

		//diamond square (midpoint displacement for 2D)
		void GenWithDS(SVec4 corners, unsigned int steps, float decay, float randomMax);

		//cellular automata
		void GenWithCA(float initialDistribtuion, unsigned int steps);

		//load from heightmap
		void GenFromTexture(unsigned int width, unsigned int height, const std::vector<float>& data);


		///manipulation methods

		//faulting - using z = k * x + offset
		void Fault(const SRay& line, float displacement);
		void NoisyFault(const SRay& line, float displacement);
		void TerraSlash(const SRay& line, float displacement, unsigned int steps, float decay);
		void CircleOfScorn(const SVec2& center, float radius, float angle, float displacement, unsigned int steps);
		void Smooth();	

		

		///wrapping up and directX integration
		void CalculateNormals();
		bool SetUp(ID3D11Device* device);
		void Draw(ID3D11DeviceContext* dc, ShaderBase& s, 
			const SMatrix& mt, const SMatrix& vt, const SMatrix& pt, 
			const PointLight& dLight, float deltaTime, SVec3 eyePos);


		///getters
		unsigned int	getNumCols()	{ return _numColumns;	}
		unsigned int	getNumRows()	{ return _numRows;		}
		auto&			getVerts()		{ return vertices;		}

		std::vector<SVec2> getHorizontalPositions();
	};
}

/*

private:
	bool CalculateNormals();

	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);



*/