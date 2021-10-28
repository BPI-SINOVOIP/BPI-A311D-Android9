#ifndef __ISP_3A_H__
#define __ISP_3A_H__

namespace android {
class isp3a {
	private:
		isp3a();
	private:
		static isp3a* mInstance;
		bool mIsOpened;
		int mOpenCount;
	public:
		~isp3a();
		static isp3a* get_instance(void);
		void open_isp3a_library(void);
		void close_isp3a_library(void);
		void print_status(void);
};
}
#endif
