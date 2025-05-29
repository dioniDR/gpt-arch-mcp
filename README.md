# 🚀 GPT Arch Linux Assistant with MCP

Standalone intelligent terminal assistant combining GPT conversational AI with Model Context Protocol (MCP) for direct system command execution. Specialized for Arch Linux administration and maintenance.

## ✨ Features

- **🤖 Direct Command Execution**: Type commands like `ls`, `pacman -Syu` and they execute automatically
- **💬 GPT Integration**: Ask questions and get intelligent responses with command suggestions  
- **🌉 MCP Bridge**: Native AOT compiled bridge with zero .NET runtime dependencies
- **📦 Self-Contained**: All dependencies included in compiled module
- **🔧 Arch Linux Specialized**: Optimized for Arch Linux system administration

## 🎯 Quick Usage Examples

```bash
🤖 > ls -la          # Executes directly
🤖 > df -h           # Shows disk usage immediately  
🤖 > pacman -Syu     # Updates system with confirmation
🤖 > "How to install Docker?"  # GPT explains + suggests commands
🤖 > /status         # System information via MCP
🤖 > /diag           # Complete Arch diagnostics
```

## 📦 Dependencies

### Build Requirements
- **.NET 8.0 SDK** (for compiling MCP bridge)
- **GCC** (for C compilation)
- **jq, curl** (for JSON processing and API calls)

### Runtime Requirements  
- **jq, curl** only (bridge has no .NET dependencies after compilation)

## 🚀 Quick Start

```bash
# 1. Install build dependencies
sudo pacman -S dotnet-sdk gcc jq curl

# 2. Clone and build
git clone https://github.com/dioniDR/gpt-arch-mcp.git
cd gpt-arch-mcp
make check_deps
make arch_mcp

# 3. Configure and run
cd out/gpt-arch-mcp/
cp api/config.txt.example api/config.txt
nano api/config.txt  # Add your OpenAI API key
./run.sh
```

## 🔧 Build Commands

```bash
make arch_mcp      # Build complete assistant
make build_bridge  # Build only MCP bridge  
make test_bridge   # Test MCP functionality
make check_deps    # Verify dependencies
make clean         # Clean build artifacts
make help          # Show all commands
```

## 📁 Output Structure

After compilation, you get a self-contained module:

```
out/gpt-arch-mcp/
├── gpt_arch_mcp           # Main executable (40KB)
├── MCPBridge_native       # MCP bridge (3.7MB, no .NET deps)
├── api/config.txt.example # API configuration template
├── modulos/arch_mcp/      # Module configuration
└── run.sh                # Smart launcher script
```

## 📋 Self-Contained Benefits

- **Zero external dependencies** after compilation
- **Easy distribution** - just copy the `out/gpt-arch-mcp/` folder
- **Works on any Linux** system with jq and curl
- **No .NET runtime** required on target systems

## 🎮 Usage

The assistant automatically detects whether you're:
- **Typing a command** → Executes directly with MCP
- **Asking a question** → Sends to GPT for intelligent response
- **Using special commands** → Built-in system functions

### Special Commands
- `/help` - Show all commands
- `/status` - System information
- `/diag` - Arch Linux diagnostics  
- `/clear` - Clear conversation context
- `/mcp` - MCP bridge status

## 🔒 Security

- API keys are never committed (gitignored)
- Bridge validates dangerous commands
- All system calls are logged
- Configurable command restrictions

## 📄 License

MIT License - Feel free to use in your projects!

---

**Perfect for Arch Linux enthusiasts who want GPT assistance with direct system integration!**
