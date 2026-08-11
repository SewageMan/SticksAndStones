#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

class CppCore : public Object {
    GDCLASS(CppCore, Object)

private:
    int _count = 0;

protected:
    static void _bind_methods() {
        ClassDB::bind_method(D_METHOD("initialise"), &CppCore::initialise);
    }

public:
    void initialise() {

    }
};