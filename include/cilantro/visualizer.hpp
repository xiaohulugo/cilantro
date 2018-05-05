#pragma once

#include <pangolin/display/display_internal.h>
#include <cilantro/renderable.hpp>
#include <cilantro/visualizer_handler.hpp>
#include <cilantro/space_transformations.hpp>

namespace cilantro {
    class Visualizer {
        friend class VisualizerHandler;
    public:
        EIGEN_MAKE_ALIGNED_OPERATOR_NEW

        Visualizer();

        Visualizer(const std::string &window_name, const std::string &display_name);

        ~Visualizer();

        template <class RenderableT>
        Visualizer& addObject(const std::string &name, std::shared_ptr<RenderableT> obj) {
            gl_context_->MakeCurrent();
            auto& mr = (renderables_[name] = ManagedRenderable(obj, std::shared_ptr<typename RenderableT::GPUBuffers>(new typename RenderableT::GPUBuffers())));
            mr.first->setRenderingProperties(*(renderables_[name].second));
            return *this;
        }

        template <class RenderableT, class... Args>
        Visualizer& addObject(const std::string &name, Args... args) {
            gl_context_->MakeCurrent();
            auto& mr = (renderables_[name] = ManagedRenderable(std::shared_ptr<RenderableT>(new RenderableT(args...)), std::shared_ptr<typename RenderableT::GPUBuffers>(new typename RenderableT::GPUBuffers())));
            mr.first->setRenderingProperties(*(renderables_[name].second));
            return *this;
        }

        template <class RenderableT = Renderable>
        std::shared_ptr<RenderableT> getObject(const std::string &name) {
            auto it = renderables_.find(name);
            if (it == renderables_.end()) return std::shared_ptr<RenderableT>();
            return std::dynamic_pointer_cast<RenderableT>(it->second.first);
        }

        RenderingProperties getRenderingProperties(const std::string &name) const;

        Visualizer& setRenderingProperties(const std::string &name, const RenderingProperties &rp);

        Visualizer& setRenderingProperties(const std::string &name);

        inline Visualizer& updateObject(const std::string &name) { return setRenderingProperties(name); }

        Visualizer& updateAllObjects();

        bool getVisibility(const std::string &name) const;

        Visualizer& setVisibility(const std::string &name, bool visible);

        Visualizer& toggleVisibility(const std::string &name);

        Visualizer& clear();

        Visualizer& remove(const std::string &name);

        Visualizer& clearRenderArea();

        Visualizer& render();

        Visualizer& finishFrame();

        Visualizer& spinOnce();

        inline bool wasStopped() const { return gl_context_->quit; }

        std::vector<std::string> getObjectNames() const;

        inline Eigen::Vector3f getClearColor() const { return clear_color_; }

        inline Visualizer& setClearColor(const Eigen::Vector3f &color) { clear_color_ = color; return *this; }

        inline Visualizer& setClearColor(float r, float g, float b) { clear_color_ << r, g, b; return *this; }

        Visualizer& setPerspectiveProjectionMatrix(size_t w, size_t h,
                                                   pangolin::GLprecision fu, pangolin::GLprecision fv,
                                                   pangolin::GLprecision u0, pangolin::GLprecision v0,
                                                   pangolin::GLprecision zNear, pangolin::GLprecision zFar);

        Visualizer& setPerspectiveProjectionMatrix(size_t w, size_t h,
                                                   const Eigen::Matrix3f &intrinsics,
                                                   pangolin::GLprecision zNear, pangolin::GLprecision zFar);

        Visualizer& setOrthographicProjectionMatrix(pangolin::GLprecision left, pangolin::GLprecision right,
                                                    pangolin::GLprecision bottom, pangolin::GLprecision top,
                                                    pangolin::GLprecision near, pangolin::GLprecision far);

        Visualizer& setOrthographicProjectionMatrix(pangolin::GLprecision height,
                                                    pangolin::GLprecision near, pangolin::GLprecision far);

        inline Visualizer& enablePerspectiveProjection() { input_handler_->EnablePerspectiveProjection(); return *this; }

        inline Visualizer& enableOrthographicProjection() { input_handler_->EnableOrthographicProjection(); return *this; }

        inline Visualizer& toggleProjectionMode() { input_handler_->ToggleProjectionMode(); return *this; }

        const Visualizer& getCameraPose(Eigen::Ref<Eigen::Matrix4f> pose) const;

        inline const Visualizer& getCameraPose(RigidTransformation3f &pose) const { return getCameraPose(pose.matrix()); }

        const Visualizer& getCameraPose(Eigen::Vector3f &position,
                                        Eigen::Vector3f &look_at,
                                        Eigen::Vector3f &up_direction) const;

        Visualizer& setCameraPose(const Eigen::Vector3f &position,
                                  const Eigen::Vector3f &look_at,
                                  const Eigen::Vector3f &up_direction);

        Visualizer& setCameraPose(float pos_x, float pos_y, float pos_z,
                                  float look_at_x, float look_at_y, float look_at_z,
                                  float up_dir_x, float up_dir_y, float up_dir_z);

        Visualizer& setCameraPose(const Eigen::Ref<const Eigen::Matrix4f> &pose);

        inline Visualizer& setCameraPose(const RigidTransformation3f &pose) { return setCameraPose(pose.matrix()); }

        const Visualizer& getDefaultCameraPose(Eigen::Ref<Eigen::Matrix4f> pose) const;

        inline const Visualizer& getDefaultCameraPose(RigidTransformation3f &pose) const { return getDefaultCameraPose(pose.matrix()); }

        const Visualizer& getDefaultCameraPose(Eigen::Vector3f &position,
                                               Eigen::Vector3f &look_at,
                                               Eigen::Vector3f &up_direction) const;

        Visualizer& setDefaultCameraPose(const Eigen::Vector3f &position,
                                         const Eigen::Vector3f &look_at,
                                         const Eigen::Vector3f &up_direction);

        Visualizer& setDefaultCameraPose(float pos_x, float pos_y, float pos_z,
                                         float look_at_x, float look_at_y, float look_at_z,
                                         float up_dir_x, float up_dir_y, float up_dir_z);

        Visualizer& setDefaultCameraPose(const Eigen::Ref<const Eigen::Matrix4f> &pose);

        inline Visualizer& setDefaultCameraPose(const RigidTransformation3f &pose) { return setDefaultCameraPose(pose.matrix()); }

        Visualizer& registerKeyboardCallback(unsigned char key, std::function<void(void)> func);

        pangolin::TypedImage getRenderImage(float scale = 1.0f, bool rgba = false);

        Visualizer& saveRenderAsImage(const std::string &file_name, float scale, float quality, bool rgba = false);

        Visualizer& startVideoRecording(const std::string &uri, size_t fps,
                                        bool record_on_render = false, float scale = 1.0f, bool rgba = false);

        Visualizer& recordVideoFrame();

        Visualizer& stopVideoRecording();

        inline bool isRecording() const { return !!video_recorder_; }

        inline pangolin::PangolinGl* getGLContext() const { return gl_context_; }

        inline pangolin::View* getDisplay() const { return display_; }

        inline pangolin::OpenGlRenderState* getRenderState() const { return gl_render_state_.get(); }

        inline VisualizerHandler* getInputHandler() const { return input_handler_.get(); }

    private:
        pangolin::PangolinGl *gl_context_;
        pangolin::View *display_;

        std::shared_ptr<pangolin::OpenGlRenderState> gl_render_state_;
        std::shared_ptr<VisualizerHandler> input_handler_;
        std::shared_ptr<pangolin::VideoOutput> video_recorder_;
        size_t video_fps_;
        float video_scale_;
        bool video_rgba_;
        bool video_record_on_render_;
        Eigen::Vector3f clear_color_;
        Eigen::Matrix4f cam_axes_rot_;

        typedef std::pair<std::shared_ptr<Renderable>,std::shared_ptr<GPUBufferObjects>> ManagedRenderable;

        std::map<std::string,ManagedRenderable> renderables_;

        void init_(const std::string &window_name, const std::string &display_name);

        struct RenderPriorityComparator_ {
            const Eigen::Matrix3f& rot_mat_;
            const Eigen::Vector3f& t_vec_;

            inline RenderPriorityComparator_(const Eigen::Matrix3f &R, const Eigen::Vector3f &t)
                    : rot_mat_(R), t_vec_(t)
            {}

            inline bool operator()(const ManagedRenderable* o1, const ManagedRenderable* o2) const {
                return std::make_tuple(!o1->first->getDrawLast(),
                                       o1->first->getRenderingProperties().opacity == 1.0f,
                                       (rot_mat_*(o1->first->getCentroid())+t_vec_).squaredNorm()) >
                       std::make_tuple(!o2->first->getDrawLast(),
                                       o2->first->getRenderingProperties().opacity == 1.0f,
                                       (rot_mat_*(o2->first->getCentroid())+t_vec_).squaredNorm());
            }
        };
    };
}
