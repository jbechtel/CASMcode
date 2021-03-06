#ifndef MOLECULE_HH
#define MOLECULE_HH

#include <iostream>

#include "casm/basis_set/DoF.hh"
#include "casm/crystallography/Coordinate.hh"
#include "casm/symmetry/SymOp.hh"

namespace CASM {

  class Molecule;
  template <typename T>
  class OccupantDoF;
  typedef OccupantDoF<Molecule> MoleculeOccupant;
  //****************************************************

  class Specie {
  public:
    std::string name;
    double mass, magmom, U, J; //Changed 05/10/13 -- was int
    Specie() { };
    explicit Specie(std::string init_name) : name(init_name) { };

    bool is_vacancy() const {
      return (name == "VA" || name == "va" || name == "Va");
    }


    bool operator==(const Specie &RHS) const {
      return
        (name == RHS.name);
    };

  };

  jsonParser &to_json(const Specie &specie, jsonParser &json);

  void from_json(Specie &specie, const jsonParser &json);

  //****************************************************

  class AtomPosition : public Coordinate {
  public:
    Specie specie;

    Vector3<bool> SD_flag;

    explicit AtomPosition(const Lattice &init_lattice) : Coordinate(Vector3<double>(0, 0, 0), init_lattice) { };
    AtomPosition(double elem1, double elem2, double elem3, std::string sp_name, const Lattice &init_lattice) :
      Coordinate(Vector3<double>(elem1, elem2, elem3), init_lattice),
      specie(sp_name) { };

    AtomPosition(double elem1, double elem2, double elem3, std::string sp_name, const Lattice &init_lattice, const Vector3<bool> &tSD_flag) :
      Coordinate(Vector3<double>(elem1, elem2, elem3), init_lattice),
      specie(sp_name),
      SD_flag(tSD_flag) { };


    bool operator==(const AtomPosition &RHS) const;

    void print(std::ostream &stream, const Coordinate &trans, int spaces, bool SD_is_on = false) const;
    // TODO: If comparing coordinates alone does not suffice, add a == operator here.

    AtomPosition &apply_sym(const SymOp &op);
    AtomPosition &apply_sym_no_trans(const SymOp &op);


  };

  jsonParser &to_json(const AtomPosition &apos, jsonParser &json);

  // Lattice must be set already
  void from_json(AtomPosition &apos, const jsonParser &json);

  //****************************************************

  class Molecule : public Array<AtomPosition> {

    Lattice const *m_home;

    Array<SymOp> point_group;

  public:
    using Array<AtomPosition>::push_back;
    using Array<AtomPosition>::at;
    using Array<AtomPosition>::size;

    Coordinate center;
    std::string name;

    explicit Molecule(const Lattice &init_home) : m_home(&init_home), center(Vector3<double>(0, 0, 0), init_home) {};

    Lattice const *home() const {
      return m_home;
    }

    void get_center(); //TODO

    void generate_point_group(); //TODO

    bool is_vacancy() const {
      return (name == "VA" || name == "va" || name == "Va");
    };

    Molecule &apply_sym(const SymOp &op); //TODO
    Molecule &apply_sym_no_trans(const SymOp &op); //TODO

    void invalidate(COORD_TYPE mode);
    void set_lattice(const Lattice &new_lat);

    Molecule get_union(const Molecule &RHS); //TODO

    Molecule &operator*=(const SymOp &RHS); //TODO
    Molecule &operator+=(const Coordinate &RHS); //TODO

    bool operator==(const Molecule &RHS) const;
    bool contains(const std::string &name) const;

    void read(std::istream &stream);
    void print(std::ostream &stream, const Coordinate &trans, int spaces, char delim, bool SD_is_on = false) const;

    jsonParser &to_json(jsonParser &json) const;

    // Lattice must be set already
    void from_json(const jsonParser &json);
  };

  Molecule operator*(const SymOp &LHS, const Molecule &RHS); //TODO
  Molecule operator+(const Coordinate &LHS, const Molecule &RHS); //TODO
  Molecule operator+(const Molecule &LHS, const Coordinate &RHS); //TODO



  jsonParser &to_json(const Molecule &mol, jsonParser &json);

  // Lattice must be set already
  void from_json(Molecule &mol, const jsonParser &json);

};
#endif
