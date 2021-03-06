#ifndef ORBITREE_HH
#define ORBITREE_HH

#include <iostream>
#include <fstream>
#include <complex>

#include "casm/clusterography/OrbitBranch.hh"
#include "casm/basis_set/BasisSet.hh"
#include "casm/crystallography/Lattice.hh"

namespace CASM {

  using Eigen::MatrixXcd;


  //Orbitree is an (outer) array of N (inner) arrays of Orbit, where N-1 is the maximum number of sites of any cluster
  //The n^th (inner) Array of Orbitree has all clusters that contain 'n' sites

  class Configuration;

  class Supercell;

  template<typename ClustType>
  class GenericOrbitree;

  class SiteCluster;
  typedef GenericOrbitree<SiteCluster> SiteOrbitree;

  class HopCluster;
  typedef GenericOrbitree<HopCluster> HopOrbitree;

  template<typename ClustType>
  class GenericOrbitree : public Array< GenericOrbitBranch<ClustType> > {
  public:
    using Array< GenericOrbitBranch<ClustType> > :: size;
    using Array< GenericOrbitBranch<ClustType> > :: at;
    using Array< GenericOrbitBranch<ClustType> > :: back;
    using Array< GenericOrbitBranch<ClustType> > :: operator[];

    Lattice lattice;

    //generative properties of Orbitree

    Index max_num_sites, min_num_components;

    ///Maximum allowed between any two points in a cluster of Orbitree[i]
    ///(e.g., max_length[i] is the max length allowed for a cluster in the 'i' OrbitBranch)
    Array<double> max_length;
    /// minimum length allowed between any two points in a cluster
    /// if negative, N-clusters with repeated points are allowed
    double min_length;
    ///we don't use num_clusts yet.  It might be a better way to specify how many clusters to enumerate
    Array<int> num_clusts;

    /// Arrays to map 2-D orbit indices onto a single index
    /// for my_orbitree[i][j], there is a unique index k=index[i][j]
    /// 'i' can be found using index_to_row[k]
    /// 'j' can be found using index_to_column[k]
    mutable Array<int> index_to_row, index_to_column;
    mutable Array< Array<int> > index;
    mutable Index Norbits; // Brian: set this in get_index()

    /// For each orbit 'k', lists all orbits that contain subclusters of
    /// the clusters in orbit 'k' (given by their linear index)
    mutable Array< Array<int> > subcluster;

    /// phenom_clust is the 'phenomenal cluster' around which local clusters are obtained
    /// When generating a local orbitree, the symmetry properties of phenom_cluster are used
    /// BP- Now phenom_clust is only a temporary object in generate_local_orbitree()
    // ClustType phenom_clust;

    //BEGIN METHODS OF ORBITREE:

    GenericOrbitree<ClustType>(const Lattice &t_lat) : lattice(t_lat), min_length(TOL) { };
    //John G 011013 COPY CONSTRUCTOR
    GenericOrbitree<ClustType>(const GenericOrbitree<ClustType> &starttree);


    ///Access orbits using 2-D indexing
    GenericOrbit<ClustType> &orbit(Index np, Index no);
    const GenericOrbit<ClustType> &orbit(Index np, Index no) const;

    ///Access prototype of orbit (np, no)
    const ClustType &prototype(Index np, Index no) const;
    ClustType &prototype(Index np, Index no);

    ///Access equivalent cluster 'ne' of orbit (np, no)
    const ClustType &equiv(Index np, Index no, Index ne) const;
    ClustType &equiv(Index np, Index no, Index ne);

    /// Number of orbits in OrbitBranch 'np'
    Index size(Index np) const;
    /// Number of equivalent clusters in Orbit (np, no)
    Index size(Index np, Index no) const;

    /// Count number of basis functions at each orbit and sum result
    Index basis_set_size() const;

    /// Initialize NP orbitbranches in the Orbitree.  Any existing orbits get deleted.
    void resize(Index NP);

    /// push_back a new orbit. If orbit has clusters of N sites, it will be placed in lowest-index
    /// OrbitBranch that contains N-clusters.  If none is found, a new OrbitBranch is added
    void push_back(const GenericOrbit<ClustType> &new_orbit);

    /// push_back new OrbitBranch.  Verifies that new branch uses this->lattice as home lattice
    void push_back(const GenericOrbitBranch<ClustType> &new_branch);

    /// sets lattice=new_lat and also updates all OrbitBranches, Orbits, and Clusters
    void set_lattice(const Lattice &new_lat, COORD_TYPE mode);

    ///Populates 'index', 'index_to_row' and 'index_to_column' Arrays
    void get_index() const;  //Done - Alex

    ///Calls 'sort()' on each OrbitBranch to sort Orbits by lengthscale
    void sort(); //Done - Alex
    ///Calls 'sort()' on the np OrbitBranch
    void sort(Index np); //Done - Alex

    /// Set basis_ind() of each site of each cluster in GenericOrbitree<ClustType>
    /// using the order of basis sites in 'struc'
    void collect_basis_info(const Structure &struc, const Coordinate &shift);
    void collect_basis_info(const Structure &struc);

    /// Call get_s2s_vec on all clusters in orbitree
    void get_s2s_vec();

    ///TODO: Search through (*this) and get one Orbitree for each basis site such that
    /// out_trees[i] contains all clusters radiating from basis site 'i'
    void get_clusters_by_site(Array<GenericOrbitree<ClustType> > &out_trees) const;

    /// get clust_basis for all equivalent clusters assuming configurational DoFs
    void generate_config_clust_bases();

    /// get clust_basis for all equivalent clusters
    void generate_clust_bases(const Array<BasisSet const *> &global_args, Index max_poly_order = -1);
    void generate_clust_bases(Index max_poly_order = -1);

    /// fill up cluster function evaluation tensors for every cluster
    void fill_discrete_bases_tensors();

    // Alex do these (there's already a placeholder for the first one):
    ///If cluster/orbit exists in current orbitree, return its linear index; else, return number of orbits in orbitree
    Index find(const ClustType &test_clust) const;
    Index find(const GenericOrbit<ClustType> &test_orbit) const;
    Index find(const ClustType &test_clust, Index nb) const;

    ///If cluster exists in current orbitree, return true
    bool contains(const ClustType &test_clust);
    ///If orbit exists in current orbitree, return true
    bool contains(const GenericOrbit<ClustType> &test_orbit);
    //bool tmp_contains(const ClustType &test_clust);

    //Finds all the clusters of a structure, based on the generative properties
    void generate_orbitree(const Structure &prim, bool verbose = false); //John
    void generate_orbitree_TB(const Structure &prim); //AAB
    void generate_orbitree(const Structure &prim, const int maxClust); //Anirudh
    void generate_orbitree_neighbour(const Structure &prim, const Array<int> maxNeighbour); //Anirudh

    void generate_decorated_orbitree(const GenericOrbitree<ClustType> &in_tree, const SymGroup &symgroup, PERIODICITY_TYPE ptype, bool full_decor = false); //Brian
    void generate_hop_orbitree(const GenericOrbitree<SiteCluster> &in_tree, const Structure &prim); //Brian

    void generate_orbitree_from_proto_file(std::string filename, const SymGroup &sym_group, PERIODICITY_TYPE ptype);

    //Finds the shortest non-overlapping clusters within a supercell
    void generate_in_cell(const Structure &prim, const Lattice &cell, int num_images = 0); //John

    //Finds all the clusters around a phenomenal cluster
    //void generate_local_orbitree(const ClustType &phenomenal_clust, const Structure &prim); //John

    //Get clusters of size 'num_sites' radiating from each site in the basis
    void get_clusters_by_site(const GenericOrbitree<ClustType> &in_tree, GenericOrbitree<ClustType> &out_tree, int num_sites);


    //finds the subclusters of a given cluster
    void get_hierarchy() const; //Ivy

    void read(std::istream &stream, int num_sites, COORD_TYPE mode);
    void read(std::istream &stream, COORD_TYPE mode);
    ///Reads in tensor basis and ECI's of prototypes of each orbit
    void read_prototype_tensor_basis(std::istream &stream, COORD_TYPE mode, const SymGroup &sym_group);//Modified by Ivy
    void read_CSPECS(std::istream &stream); //Added by Ivy -- temporary?


    void print(std::ostream &stream) const;
    ///Writes the tensor basis of all the np size clusters to a file
    void write_full_tensor_basis(std::string file, Index np) const; //Added by Ivy
    ///Writes the prototype tensor basis of np size clusters to file
    void write_prototype_tensor_basis(std::string file, Index np, std::string path = "") const; //Added by Ivy
    ///Calculates the force constant tensors for each cluster in the Orbitree
    void calc_tensors();

    void write_full_clust(std::string file) const;
    void write_proto_clust(std::string file) const;
    void write_full_decorated_clust(std::string file) const;
    void write_proto_decorated_clust(std::string file) const;

    void print_full_clust(std::ostream &out) const;
    void print_full_basis_info(std::ostream &out) const;
    void print_full_clust_basis(std::ostream &out) const;
    void print_proto_clust(std::ostream &out) const;
    void print_full_decorated_clust(std::ostream &out) const;
    void print_proto_decorated_clust(std::ostream &out) const;

    void write_eci_in(std::string filename) const;
    void print_eci_in(std::ostream &out) const;

    //Array <double> calc_correlations(Supercell super_calc);

    void get_dynamical_matrix(MatrixXcd &dmat, const Coordinate &k, Index bands_per_site);

    void apply_sym(const SymOp &op);
    template<class PhenomType> void generate_local_orbitree(const Structure &prim, const PhenomType &tmp_phenom_clust, bool include_phenom_clust_sites);

    void trans_and_expand(const Array<Coordinate> &prim_grid, bool override = 0);

    GenericOrbitree<ClustType> &operator=(const GenericOrbitree<ClustType> &RHS);


    //jsonParser &to_json(jsonParser &json) const;

    void from_json(const jsonParser &json);
    void read_orbitree_from_json(const std::string &json_file_name, const SymGroup &sym_group, const Structure &ref_struc);
    bool read_custom_clusters_from_json(const jsonParser &json,
                                        const Structure &struc,
                                        const SymGroup &sym_group,
                                        bool verbose = false);

    void add_subclusters(const ClustType &big_clust, const Structure &prim, bool verbose = false);
  };

  template<typename ClustType>
  std::ostream &operator<< (std::ostream &stream, const GenericOrbitree<ClustType> &orbitree);

  /*
  template<typename ClustType>
  jsonParser &to_json(const GenericOrbitree<ClustType> &tree, jsonParser &json) {
    return tree.to_json(json);
  }
  */

  /// Assumes the prototype lattice is already set
  template<typename ClustType>
  void from_json(GenericOrbitree<ClustType> &tree, const jsonParser &json) {
    tree.from_json(json);
  }
};
#include "casm/clusterography/Orbitree_impl.hh"
#endif
