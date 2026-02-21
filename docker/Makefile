.PHONY: run ci-test

run:
	@echo "Granting X11 access to docker..."
	@xhost +local:docker >/dev/null
	@echo "Building container..."
	@docker compose -f docker-compose.yml build
	@echo "Running container..."
	@docker compose -f docker-compose.yml run --rm mc-rtc
	@echo "Revoking X11 access..."
	@xhost -local:docker >/dev/null

ci-test:
	@echo "Building CI container..."
	@docker compose -f docker-compose.ci.yml build
	@echo "Running CI build..."
	@docker compose -f docker-compose.ci.yml run --rm mc-rtc-ci \
		bash -c "rm -rf build && cmake -S . -B build -G Ninja \
			-DCMAKE_BUILD_TYPE=RelWithDebInfo \
			-DCMAKE_C_COMPILER_LAUNCHER=ccache \
			-DCMAKE_CXX_COMPILER_LAUNCHER=ccache \
			-DPYTHON_BINDING=OFF && \
			cmake --build build && \
			cmake --install build && \
			ctest --test-dir build -V && \
			echo 'CI test passed!'"
