# GPT Arch Linux Assistant with MCP - Standalone Build
CC = gcc
CFLAGS = -Wall -Wextra -DMODO_ARCH_MCP
INCLUDES = -I. -Isrc -Isrc/common/includes -Iapi -Imodulos/arch_mcp

# Directorio de salida limpio
OUT_DIR = out

# Archivos fuente
MAIN_SRC = src/main_mcp.c
COMMON_SRCS = src/common/utils.c src/common/config_manager.c src/common/context.c
API_SRCS = api/openai.c
MCP_SRCS = src/mcp_client.c
MODULE_SRCS = modulos/arch_mcp/executor.c

# Variables MCP
MCP_BRIDGE = MCPBridge
MCP_BRIDGE_NATIVE = $(OUT_DIR)/MCPBridge_native

# Objetivo por defecto
all: arch_mcp

# Crear directorio de salida
$(OUT_DIR):
	@mkdir -p $(OUT_DIR)

# Compilar bridge MCP nativo
build_bridge: $(OUT_DIR)
	@echo "🔨 Compilando MCP Bridge nativo..."
	@if [ ! -f $(MCP_BRIDGE).csproj ]; then \
		echo "❌ No se encontró MCPBridge.csproj"; \
		exit 1; \
	fi
	@echo "📦 Compilando a código nativo..."
	@dotnet publish $(MCP_BRIDGE).csproj -r linux-x64 -c Release -p:PublishAot=true -o $(OUT_DIR)/
	@if [ -f $(OUT_DIR)/$(MCP_BRIDGE) ]; then \
		mv $(OUT_DIR)/$(MCP_BRIDGE) $(MCP_BRIDGE_NATIVE); \
		chmod +x $(MCP_BRIDGE_NATIVE); \
		echo "✅ Bridge compilado: $(MCP_BRIDGE_NATIVE)"; \
	else \
		echo "❌ Error al compilar el bridge"; \
		exit 1; \
	fi

# Compilar módulo arch_mcp auto-contenido
arch_mcp: build_bridge
	@echo "🔨 Compilando GPT Arch Linux Assistant..."
	
	# Compilar ejecutable principal
	$(CC) $(CFLAGS) $(INCLUDES) \
		-o $(OUT_DIR)/gpt_arch_mcp \
		$(MAIN_SRC) $(COMMON_SRCS) $(API_SRCS) $(MCP_SRCS) $(MODULE_SRCS)
	
	# Crear módulo auto-contenido
	@echo "📦 Creando módulo auto-contenido..."
	@mkdir -p $(OUT_DIR)/gpt-arch-mcp/api
	@mkdir -p $(OUT_DIR)/gpt-arch-mcp/modulos/arch_mcp
	
	# Copiar archivos esenciales
	@cp $(OUT_DIR)/gpt_arch_mcp $(OUT_DIR)/gpt-arch-mcp/
	@cp $(MCP_BRIDGE_NATIVE) $(OUT_DIR)/gpt-arch-mcp/
	@cp api/config.txt.example $(OUT_DIR)/gpt-arch-mcp/api/
	@cp modulos/arch_mcp/config.ini modulos/arch_mcp/role.txt $(OUT_DIR)/gpt-arch-mcp/modulos/arch_mcp/
	
	# Crear script de ejecución
	@echo "#!/bin/bash" > $(OUT_DIR)/gpt-arch-mcp/run.sh
	@echo "cd \"\$$(dirname \"\$$0\")\"" >> $(OUT_DIR)/gpt-arch-mcp/run.sh
	@echo "" >> $(OUT_DIR)/gpt-arch-mcp/run.sh
	@echo "echo '🔍 Verificando dependencias...'" >> $(OUT_DIR)/gpt-arch-mcp/run.sh
	@echo "which jq >/dev/null || (echo '❌ Instala jq: sudo apt install jq'; exit 1)" >> $(OUT_DIR)/gpt-arch-mcp/run.sh
	@echo "which curl >/dev/null || (echo '❌ Instala curl: sudo apt install curl'; exit 1)" >> $(OUT_DIR)/gpt-arch-mcp/run.sh
	@echo "" >> $(OUT_DIR)/gpt-arch-mcp/run.sh
	@echo "if [ ! -f api/config.txt ]; then" >> $(OUT_DIR)/gpt-arch-mcp/run.sh
	@echo "  echo '⚠️  Configura tu API key:'" >> $(OUT_DIR)/gpt-arch-mcp/run.sh
	@echo "  echo '  cp api/config.txt.example api/config.txt'" >> $(OUT_DIR)/gpt-arch-mcp/run.sh
	@echo "  echo '  nano api/config.txt'" >> $(OUT_DIR)/gpt-arch-mcp/run.sh
	@echo "  exit 1" >> $(OUT_DIR)/gpt-arch-mcp/run.sh
	@echo "fi" >> $(OUT_DIR)/gpt-arch-mcp/run.sh
	@echo "" >> $(OUT_DIR)/gpt-arch-mcp/run.sh
	@echo "echo '🚀 GPT Arch Linux Assistant con MCP'" >> $(OUT_DIR)/gpt-arch-mcp/run.sh
	@echo "./gpt_arch_mcp \"\$$@\"" >> $(OUT_DIR)/gpt-arch-mcp/run.sh
	@chmod +x $(OUT_DIR)/gpt-arch-mcp/run.sh
	
	@echo "✅ GPT Arch Linux Assistant compilado"
	@echo "📁 Módulo auto-contenido: $(OUT_DIR)/gpt-arch-mcp/"
	@echo "🚀 Para usar: cd $(OUT_DIR)/gpt-arch-mcp && ./run.sh"

# Probar bridge MCP
test_bridge:
	@echo "🧪 Probando MCP Bridge..."
	@if [ -f $(MCP_BRIDGE_NATIVE) ]; then \
		echo '{"Action":"get_system_info"}' | $(MCP_BRIDGE_NATIVE) | head -n 1 | grep -q "Success" \
		&& echo "✅ Bridge funciona correctamente" \
		|| echo "❌ Bridge no responde"; \
	else \
		echo "❌ Bridge no encontrado. Ejecuta 'make build_bridge'"; \
	fi

# Verificar dependencias
check_deps:
	@echo "🔍 Verificando dependencias..."
	@which dotnet > /dev/null || (echo "❌ .NET SDK no instalado"; exit 1)
	@echo "✅ .NET $$(dotnet --version)"
	@which gcc > /dev/null || (echo "❌ GCC no instalado"; exit 1)
	@echo "✅ GCC instalado"
	@which jq > /dev/null || (echo "❌ jq no instalado"; exit 1)
	@echo "✅ jq instalado"
	@which curl > /dev/null || (echo "❌ curl no instalado"; exit 1)
	@echo "✅ curl instalado"

# Limpiar archivos compilados
clean:
	@echo "🧹 Limpiando..."
	rm -rf $(OUT_DIR)/
	rm -rf bin/ obj/
	@echo "✅ Archivos limpiados"

# Ayuda
help:
	@echo "🚀 GPT Arch Linux Assistant - Comandos disponibles:"
	@echo "  make arch_mcp      - Compilar asistente completo"
	@echo "  make build_bridge  - Compilar solo el bridge MCP"
	@echo "  make test_bridge   - Probar bridge MCP"
	@echo "  make check_deps    - Verificar dependencias"
	@echo "  make clean         - Limpiar archivos compilados"
	@echo "  make help          - Mostrar esta ayuda"

.PHONY: all arch_mcp build_bridge test_bridge check_deps clean help
