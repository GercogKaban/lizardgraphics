namespace LGraphics
{
	class LApp;
	class LPicking
	{
		friend LApp;
	public:
		LPicking(LApp* app, size_t width, size_t height);
		~LPicking();
	private:

		int colorBuffer;
		uint32_t depthText, colorText, fbo;
		size_t width,height;
	};
}