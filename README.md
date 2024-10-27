# GrassGL
#### A GPU Instancing Demo
![output](https://github.com/user-attachments/assets/ee549a8f-8176-4ca6-a045-ff5510590b6c)

## Build

This project relies on [**vcpkg**](https://vcpkg.io/en/) to resolve dependencies which is included in **Visual Studio 2022 [17.6](https://devblogs.microsoft.com/cppblog/vcpkg-is-now-included-with-visual-studio/)** and above
### Integrate vcpkg (NOTE: this only needs to be done once system wide)
1. Clone the project and open the solution
2. Open **Developer Powershell** (Tools > Command Line> Developer PowerShell)
3. Run `vcpkg integrate install`

### Download dependencies and build
- Build > Build Solution or **Ctrl + Shift + B**
  - This will trigger vcpkg to install all dependencies from the manifest file, autommatically add them to include directory and link the libraries

## Run
- Hit **F5**
- Or use the interface button:

![image](https://github.com/user-attachments/assets/2e920dd2-5694-4658-8340-1b2045c40da9)
