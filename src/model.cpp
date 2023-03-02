#include "model.hpp"

#include "log.hpp"

Model::Model(std::shared_ptr<VertexArray> vao) : m_VAO(vao) {}

Model::~Model() {}

void Model::Bind() {}