/* #pragma once */

/* #include <string> */

/* #include "./renderable.hpp" */

/* namespace dl */
/* { */
/* class ShaderProgram; */

/* class Layer */
/* { */
/*  public: */
/*   Layer(std::shared_ptr<ShaderProgram> shader, const int priority = 0) : shader(shader), priority(priority) {} */
/*   virtual ~Layer() {} */

/*   virtual void render() = 0; */
/*   virtual void emplace(const std::shared_ptr<Renderable>& renderable, */
/*                        const double x, */
/*                        const double y, */
/*                        const double z) = 0; */
/*   virtual void quad(const std::shared_ptr<Renderable>& renderable, const double x, const double y, const double z) =
 * 0; */
/*   /1* virtual void init_emplacing() = 0; *1/ */
/*   /1* virtual void finalize_emplacing() = 0; *1/ */
/*   virtual bool get_should_render() = 0; */

/*  protected: */
/*   Layer() {} */
/* }; */

/* }  // namespace dl */
