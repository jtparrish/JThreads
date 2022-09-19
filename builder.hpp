#pragma once

enum JTaskDepType {
    INPUT, OUTPUT, INOUT
}

template<class T, class R>
class JTaskBuilder {
    private:
        struct JTaskDepID {

            JTaskDepType type;
            uint8_t *addr;
        }
        
        std::vector<JTaskDepID> dependencies;
        std::optional<R*> return_location;
    
    public:
        //TODO: template or void?
        template<class P>
        JTaskBuilder add_dependency(DependencyType dt, P *d);
        JTaskBuilder set_return(DependencyType dt, R *ret_loc);
        JTask run(void);

    friend class Runtime;
}

template<class T, class R>
class TaskReturnWrapper {
    T original_task;
    R *ret_location;

    void call();
}


template<class T>
class JTaskInternal : public JTaskAbstract {
    T task;
    void call(void);
}
