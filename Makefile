# Just to make sure the GUI is accessable from the host machine
docker-gui:
	@xhost +local:root
	@docker-compose up
