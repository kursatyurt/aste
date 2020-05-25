#include <mesh.hpp>

#include <fstream>
#include <sstream>
#include <boost/algorithm/string.hpp>

namespace aste {

// --- MeshName

std::string MeshName::filename() const { return _mname + ".txt"; }

std::string MeshName::connectivityfilename() const { return _mname + ".conn.txt"; }


namespace {
// Reads the main file containing the vertices and data
void readMainFile(Mesh& mesh, const std::string& filename)
{
  if (!fs::is_regular_file(filename)) {
    throw std::invalid_argument{"The mesh file does not exist"};
  }
  std::ifstream mainFile{filename};
  std::string line;
  while (std::getline(mainFile, line)){
    double x, y, z, val;
    std::istringstream iss(line);
    iss >> x >> y >> z >> val; // split up by whitespace
    std::array<double, 3> vertexPos{x, y, z};
    mesh.positions.push_back(vertexPos);
    mesh.data.push_back(val);
  }
}

// Reads the connectivity file containing the triangle and edge information
void readConnFile(Mesh& mesh, const std::string& filename)
{
  if (!fs::is_regular_file(filename)) {
    throw std::invalid_argument{"The mesh connectivity file does not exist"};
  }
  std::ifstream connFile{filename};
  std::string line;
  while (std::getline(connFile, line)){
    std::vector<std::string> parts;
    boost::split(parts, line, [](char c){ return c == ' '; });
    std::vector<size_t> indices(parts.size());
    std::transform(parts.begin(), parts.end(), indices.begin(), [](const std::string& s) -> size_t {return std::stol(s);});

    if (indices.size() == 3) {
      std::array<size_t, 3> elem{indices[0], indices[1], indices[2]};
      mesh.triangles.push_back(elem);
    } else if (indices.size() == 2) {
      std::array<size_t, 2> elem{indices[0], indices[1]};
      mesh.edges.push_back(elem);
    } else {
      throw std::runtime_error{std::string{"Invalid entry in connectivitiy file \""}.append(line).append("\"")};
    }
  }
}
}

Mesh MeshName::load() const
{
  Mesh mesh;
  readMainFile(mesh, filename());
  auto connFile = connectivityfilename();
  if (boost::filesystem::exists(connFile)) {
    readConnFile(mesh, connFile);
  }
  return mesh;
}

void MeshName::save(const Mesh& mesh) const
{
  assert(mesh.positions.size() == mesh.data.size());
  std::ofstream out(filename(), std::ios::trunc);
  out.precision(9);
  for (size_t i = 0; i < mesh.positions.size(); i++) {
    out << mesh.positions[i][0] << " "
      << mesh.positions[i][1] << " "
      << mesh.positions[i][2] << " "
      << mesh.data[i] << '\n';
  }
}

std::ostream& operator<<(std::ostream& out, const MeshName& mname) {
  return out << mname.filename();
}


// --- BaseName

MeshName BaseName::with(const ExecutionContext &context) const
{
    if (context.isParallel()) {
      return {_bname + fs::path::preferred_separator + std::to_string(context.rank)};
    } else {
      return {_bname};
    }
}

std::vector<MeshName> BaseName::findAll(const ExecutionContext &context) const
{
  // Check single timestep/meshfiles first
  // Case: a single mesh
  if (fs::is_regular_file(_bname+".txt")) {
    if (context.isParallel()) throw MeshException{"Cannot use unparitioned meshes in parallel."};
    return {MeshName{_bname + ".txt"}};
  }
  // Case: a single partitioned mesh
  if (fs::is_directory(_bname)) {
    if (!context.isParallel()) throw MeshException{"Cannot use paritioned meshes in serial."};
    std::string rankFile = _bname + std::to_string(context.rank) + ".txt";
    if (fs::is_regular_file(rankFile)) {
      return {MeshName{rankFile}};
    }
  }

  // Check multiple timesteps
  std::vector<MeshName> meshNames;
  // Case: a single mesh
  for(int t = 0; true; ++t) {
      std::string filename = _bname+".dt"+std::to_string(t)+".txt";
      if (!fs::is_regular_file(filename)) break;
      meshNames.push_back(MeshName{filename});
      if (context.isParallel()) throw MeshException{"Cannot use unparitioned meshes in parallel."};
  }
  if (!meshNames.empty()) return meshNames;


  // Case: a single mesh
  for(int t = 0; true; ++t) {
    auto stepDirectory = _bname + ".dt"+std::to_string(t);
    auto rankFile = fs::path(stepDirectory) / fs::path(std::to_string(context.rank) + ".txt");
    if (!(fs::is_directory(stepDirectory) && fs::is_regular_file(rankFile))) break;
    meshNames.push_back(MeshName{rankFile.string()});
    if (!context.isParallel()) throw MeshException{"Cannot use paritioned meshes in serial."};
  }
  return meshNames;
}

std::string Mesh::previewData(std::size_t max) const
{
  if (data.empty() || max == 0)
    return "<nothing>";

  std::stringstream oss;
  oss << data.front();
  for(size_t i = 1; i < std::min(max, data.size()); ++i)
    oss << ", " << data[i];
  oss << " ...";
  return oss.str();
}

std::string Mesh::summary() const
{
  std::stringstream oss;
  oss << positions.size() << " Vertices, " << data.size() << " Data Points, " << edges.size()  << " Edges, " << triangles.size() << " Triangles";
  return oss.str();
}

}