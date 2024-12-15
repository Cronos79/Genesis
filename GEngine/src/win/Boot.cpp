#include "Boot.h" 
#include <GEngine/src/ioc/Container.h> 
#include <GEngine/src/ioc/Singletons.h> 
#include "WindowClass.h" 
#include "Window.h"
#include "../Core/GContext.h"

// how to forward this for rval goodness? 
// how to do this for const bois? 
template<class T>
auto operator|(std::shared_ptr<T> lhs, std::shared_ptr<T> rhs)
{
	if (bool(lhs)) {
		return std::move(lhs);
	}
	else {
		return std::move(rhs);
	}
}

namespace chil::win
{
	void Boot()
	{
		// container 
		ioc::Get().Register<IWindow>([](IWindow::IocParams args) {
			return std::make_shared<Window>(
				(args.pClass | ioc::Sing().Resolve<IWindowClass>()),
				args.name.value_or(Genesis::GContext::Get().GetTitle()),
				args.size.value_or(spa::DimensionsI{ Genesis::GContext::Get().GetWidth(), Genesis::GContext::Get().GetHeight()}),
				args.position
			);
		});
		ioc::Get().Register<IWindowClass>([] { // passthru here? 
			return std::make_shared<WindowClass>();
		});

		// Singleton 
		ioc::Sing().RegisterPassthru<IWindowClass>();
	}
}
