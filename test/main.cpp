#define GOOGLE_GLOG_DLL_DECL
#define GLOG_NO_ABBREVIATED_SEVERITIES
#include <windows.h>
#include <ceres/ceres.h>
#include "glog/logging.h"

// 暂时使用另外的方式编译的glog
// 因为使用Cmake编译的库没有同步提供dll 无法使用
// 至少windows是这个样子
#pragma comment(lib, "libglog_static.lib")
#pragma comment(lib, "ceres-debug.lib")

using ceres::AutoDiffCostFunction;
using ceres::CostFunction;
using ceres::Problem;
using ceres::Solver;
using ceres::Solve;

struct CostFunctor {
	template <typename T> bool operator()(const T * const x, T* residual) const {
		residual[0] = 10.0 - x[0];
		return true;
	};
};

int main(int argc, char ** argv) {
	google::InitGoogleLogging(argv[0]);
	double x = 0.5;

	const double initial_x = x;
	Problem problem;

	CostFunction * cost_function = new AutoDiffCostFunction<CostFunctor, 1, 1>(new CostFunctor);
	problem.AddResidualBlock(cost_function, NULL, &x);
	Solver::Options options;
	options.minimizer_progress_to_stdout = true;
	Solver::Summary summary;
	Solve(options, &problem, &summary);

	std::cout << summary.BriefReport() << std::endl;
	std::cout << "x: " << initial_x << "->" << x << std::endl;

	system("pause");

	return 0;
}