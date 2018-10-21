#pragma once

#include <cinttypes>
#include <vector>
#include <type_traits>

#include "../Math/Math.hpp"

//#include <Hawk/Math/Math.hpp>


namespace Hawk {
	namespace Geometry {



		struct Vertex {
			Math::Vec3 Position;
			Math::Vec3 Normal;
			Math::Vec4 Tangent;
			Math::Vec2 Texcoord;
		};


		template<typename VertexType, typename IndexType>
		class Mesh {
		public:
			std::vector<VertexType> Vertices;
			std::vector<IndexType>  Indices;
		};

		class Generator {
		public:
			template<typename VertexType, typename IndexType>
			auto GenerateSphere(uint32_t teselation) -> Mesh<VertexType, IndexType> {
				static_assert(std::is_same<VertexType, Vertex>::value, "Undefined vertex type");
				static_assert(std::is_same<IndexType, uint16_t>::value || std::is_same<IndexType, uint32_t>::value, "Undefined index type");
			}

			template<typename VertexType, typename IndexType>
			auto GenerateQuad(uint32_t teselation) -> Mesh<VertexType, IndexType> {
				static_assert(std::is_same<VertexType, Vertex>::value, "Undefined vertex type");
				static_assert(std::is_same<IndexType, uint16_t>::value || std::is_same<IndexType, uint32_t>::value, "Undefined index type");
			}


			//template<>
			//[[nodiscard]] auto GenerateSphere(uint32_t tessellation) -> Mesh<Vertex, uint16_t> {
			//	if (tessellation < 3)
			//		throw std::out_of_range("Tesselation parameter out of range");
			//
			//	Mesh<Vertex, uint16_t> model;
			//	size_t verticalSegments = tessellation;
			//	size_t horizontalSegments = tessellation * 2;
			//
			//	for (size_t i = 0; i <= verticalSegments; i++) {
			//		auto v = 1.0f - float(i) / verticalSegments;
			//		auto latitude = (i * 3.14159265f / verticalSegments) - 3.14159265f / 2.0f;
			//
			//		auto dy = Math::Sin(latitude);
			//		auto dxz = Math::Cos(latitude);
			//
			//		for (size_t j = 0; j <= horizontalSegments; j++) {
			//			auto u = float(j) / horizontalSegments;
			//
			//			auto longitude = j * 2.0f * 3.14159265f / horizontalSegments;
			//
			//			auto dx = Math::Sin(longitude);
			//			auto dz = Math::Cos(longitude);
			//
			//			dx *= dxz;
			//			dz *= dxz;
			//
			//			auto normal = Math::Vec3(dx, dy, dz);
			//			auto uv = Math::Vec2(u, v);
			//
			//			model.Vertices.push_back({ normal, normal, Math::Vec3(0.0f, 0.0f, 0.0f), uv });
			//		}
			//	}
			//
			//	size_t stride = horizontalSegments + 1;
			//
			//	for (size_t i = 0; i < verticalSegments; i++) {
			//		for (size_t j = 0; j <= horizontalSegments; j++) {
			//
			//			size_t nextI = i + 1;
			//			size_t nextJ = (j + 1) % stride;
			//
			//			model.Indices.push_back(static_cast<uint16_t>(i * stride + j));
			//			model.Indices.push_back(static_cast<uint16_t>(nextI * stride + j));
			//			model.Indices.push_back(static_cast<uint16_t>(i * stride + nextJ));
			//
			//			model.Indices.push_back(static_cast<uint16_t>(i * stride + nextJ));
			//			model.Indices.push_back(static_cast<uint16_t>(nextI * stride + j));
			//			model.Indices.push_back(static_cast<uint16_t>(nextI * stride + nextJ));
			//
			//		}
			//	}
			//
			//	for (int index = 0; index < model.Indices.size(); index += 3) {
			//		auto const& v0 = model.Vertices[model.Indices[index + 0]].Position;
			//		auto const& v1 = model.Vertices[model.Indices[index + 1]].Position;
			//		auto const& v2 = model.Vertices[model.Indices[index + 2]].Position;
			//
			//		auto const& uv0 = model.Vertices[model.Indices[index + 0]].Texcoord;
			//		auto const& uv1 = model.Vertices[model.Indices[index + 1]].Texcoord;
			//		auto const& uv2 = model.Vertices[model.Indices[index + 2]].Texcoord;
			//
			//		auto deltaUV1 = uv1 - uv0;
			//		auto deltaUV2 = uv2 - uv0;
			//
			//		auto deltaPos1 = v1 - v0;
			//		auto deltaPos2 = v2 - v0;
			//
			//		auto f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
			//		auto t = Math::Normalize(f * (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y));
			//
			//		model.Vertices[model.Indices[index + 0]].Tangent = t;
			//		model.Vertices[model.Indices[index + 1]].Tangent = t;
			//		model.Vertices[model.Indices[index + 2]].Tangent = t;
			//	}
			//	return model;
			//}

			//template<>
			//[[nodiscard]] auto GenerateQuad(uint32_t teselation) -> Mesh<Vertex, uint16_t> {
			//
			//	Math::Vec3 pos1(-1.0f, 1.0f, 0.0f);
			//	Math::Vec3 pos2(-1.0f, -1.0f, 0.0f);
			//	Math::Vec3 pos3(1.0f, -1.0f, 0.0f);
			//	Math::Vec3 pos4(1.0f, 1.0f, 0.0f);
			//
			//	Math::Vec2 uv1(0.0f, 1.0f);
			//	Math::Vec2 uv2(0.0f, 0.0f);
			//	Math::Vec2 uv3(1.0f, 0.0f);
			//	Math::Vec2 uv4(1.0f, 1.0f);
			//
			//	Math::Vec3 nm(0.0f, 0.0f, 1.0f);
			//
			//	Math::Vec3 tangent1, bitangent1;
			//	Math::Vec3 tangent2, bitangent2;
			//
			//	Math::Vec3 edge1 = pos2 - pos1;
			//	Math::Vec3 edge2 = pos3 - pos1;
			//	Math::Vec2 deltaUV1 = uv2 - uv1;
			//	Math::Vec2 deltaUV2 = uv3 - uv1;
			//
			//	float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
			//
			//	tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
			//	tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
			//	tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
			//	tangent1 = Math::Normalize(tangent1);
			//
			//	bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
			//	bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
			//	bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
			//	bitangent1 = Math::Normalize(bitangent1);
			//
			//	// triangle 2
			//	// ----------
			//	edge1 = pos3 - pos1;
			//	edge2 = pos4 - pos1;
			//	deltaUV1 = uv3 - uv1;
			//	deltaUV2 = uv4 - uv1;
			//
			//	f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
			//
			//	tangent2.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
			//	tangent2.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
			//	tangent2.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
			//	tangent2 = Math::Normalize(tangent2);
			//
			//
			//	bitangent2.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
			//	bitangent2.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
			//	bitangent2.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
			//	bitangent2 = Math::Normalize(bitangent2);
			//
			//
			//	Mesh<Vertex, uint16_t> mesh;
			//	mesh.Vertices.push_back({ pos1, nm, tangent1, uv1 });
			//	mesh.Vertices.push_back({ pos2, nm, tangent1, uv2 });
			//	mesh.Vertices.push_back({ pos3, nm, tangent1, uv3 });
			//
			//	mesh.Vertices.push_back({ pos1, nm, tangent2, uv1 });
			//	mesh.Vertices.push_back({ pos3, nm, tangent2, uv3 });
			//	mesh.Vertices.push_back({ pos4, nm, tangent2, uv4 });
			//
			//
			//	mesh.Indices.push_back(0);
			//	mesh.Indices.push_back(1);
			//	mesh.Indices.push_back(2);
			//	mesh.Indices.push_back(3);
			//	mesh.Indices.push_back(4);
			//	mesh.Indices.push_back(5);
			//	return mesh;
			//
			//}

		};




		template<typename T>
		[[nodiscard]] auto ComputeNormal(std::vector<Math::Vec3> const& position, std::vector<T> const& indices) -> std::vector<Math::Vec3> {
			static_assert(std::is_same<T, uint16_t>::value || std::is_same<T, uint32_t>::value, "Failed indices type");

			std::vector<Math::Vec3> normals;
			for (auto index = 0; index < indices.size(); index += 3) {

				auto v0 = position[indices[index + 0]];
				auto v1 = position[indices[index + 1]];
				auto v2 = position[indices[index + 2]];

	
				auto normal = Math::Cross(Math::Normalize(v1 - v0), Math::Normalize(v2 - v0));

				normals.push_back(normal);
				normals.push_back(normal);
				normals.push_back(normal);
			}

			return normals;
		}


		template<typename T>
		[[nodiscard]] auto ComputeTexcoord(std::vector<Math::Vec3> const& position, std::vector<T> const& indices) -> std::vector<Math::Vec2> {
			static_assert(std::is_same<T, uint16_t>::value || std::is_same<T, uint32_t>::value, "Failed indices type");


			std::vector<Math::Vec2> texcoords;
			for (auto index = 0; index < position.size(); index += 4) {		
				texcoords.push_back(Math::Vec2(0.0f, 1.0f));
				texcoords.push_back(Math::Vec2(0.0f, 0.0f));
				texcoords.push_back(Math::Vec2(1.0f, 0.0f));
				texcoords.push_back(Math::Vec2(1.0f, 1.0f));
			}

			return texcoords;
		}


		template<typename T>
		[[nodiscard]]  auto ComputeTangentSpace(std::vector<Math::Vec3> const& position, std::vector<Math::Vec3> const& normals, std::vector<Math::Vec2> const& texcoord, std::vector<T> const& indices) -> std::vector<Math::Vec4> {
			static_assert(std::is_same<T, uint16_t>::value || std::is_same<T, uint32_t>::value , "Failed indices type");

			std::vector<Math::Vec4> tangents;
			auto computeTangent = [](auto const& p0, auto const& p1, auto const& uv0, auto const& uv1, auto const& n) {
				auto det = 1.0f / (uv1.x * uv1.y - uv1.y * uv1.x);
				auto t = Math::Normalize(det * (p0 * uv1.y - p1 * uv0.y));
				auto b = Math::Normalize(det * (p1 * uv0.x - p1 * uv1.x));
				return Math::Vec4(Math::Normalize(t - n * Math::Dot(n, t)), Math::Dot(Math::Cross(n, t), b) < 0.0f ? -1.0f : 1.0f);
			};

			for (auto index = 0; index < indices.size(); index += 3) {
				auto v0 = position[indices[index + 0]];
				auto v1 = position[indices[index + 1]];
				auto v2 = position[indices[index + 2]];

				auto uv0 = texcoord[indices[index + 0]];
				auto uv1 = texcoord[indices[index + 1]];
				auto uv2 = texcoord[indices[index + 2]];

				auto t0 = computeTangent(v1 - v0, v2 - v0, uv1 - uv0, uv2 - uv0, normals[indices[index + 0]]);
				auto t1 = computeTangent(v2 - v1, v0 - v1, uv2 - uv1, uv0 - uv1, normals[indices[index + 1]]);
				auto t2 = computeTangent(v0 - v2, v1 - v2, uv0 - uv2, uv1 - uv2, normals[indices[index + 2]]);

				tangents.push_back(t0);
				tangents.push_back(t1);
				tangents.push_back(t2);
			}
			return tangents;
		}


	};
}

