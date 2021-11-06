namespace LGraphics
{
	class LApp;
	class LPicking
	{
		friend LApp;
	public:
		LPicking(LApp* app, size_t size);
		~LPicking();
	private:

		int colorBuffer;
		uint32_t depthText, colorText, fbo;
		size_t size;
	};
}