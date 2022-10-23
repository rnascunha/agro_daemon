set(IMAGE_DIR		${SRC_DIR}/image)
set(IMAGE_SRC		${IMAGE_DIR}/image.cpp
					${IMAGE_DIR}/message/image.cpp
					${IMAGE_DIR}/message/process.cpp)

set(APP_DIR			${SRC_DIR}/app)
set(APP_SRC			${APP_DIR}/app.cpp
					${APP_DIR}/message/app.cpp
					${APP_DIR}/message/process.cpp)

set(MESSAGE_DIR		${SRC_DIR}/message)
set(MESSAGE_SRC		${MESSAGE_DIR}/make.cpp
					${MESSAGE_DIR}/process.cpp
					${MESSAGE_DIR}/user.cpp
					${MESSAGE_DIR}/report.cpp)

set(NOTIFY_DIR		${SRC_DIR}/notify)					
set(NOTIFY_SRC		${NOTIFY_DIR}/libs/push_notify.cpp
					${NOTIFY_DIR}/libs/telegram_bot.cpp
					${NOTIFY_DIR}/libs/mail.cpp
					${NOTIFY_DIR}/factory.cpp
					${NOTIFY_DIR}/notify.cpp
					${NOTIFY_DIR}/message/make.cpp
					${NOTIFY_DIR}/message/make_json.cpp
					${NOTIFY_DIR}/message/process.cpp)

set(DEVICE_DIR		${SRC_DIR}/device)
set(DEVICE_SRC		${DEVICE_DIR}/device.cpp
					${DEVICE_DIR}/net.cpp
					${DEVICE_DIR}/message/device.cpp
					${DEVICE_DIR}/message/process.cpp
					${DEVICE_DIR}/message/esp_reset_reason.cpp
					${DEVICE_DIR}/helper.cpp
					${DEVICE_DIR}/types.cpp
					${DEVICE_DIR}/list.cpp
					${DEVICE_DIR}/tree.cpp
					)

set(SENSOR_DIR		${SRC_DIR}/sensor)
set(SENSOR_SRC		${SENSOR_DIR}/sensor_type.cpp
					${SENSOR_DIR}/sensor_type_list.cpp
					${SENSOR_DIR}/sensor_list.cpp
					${SENSOR_DIR}/helper.cpp
					${SENSOR_DIR}/message/sensor.cpp
					${SENSOR_DIR}/message/process.cpp
					)

set(REQUEST_DIR		${DEVICE_DIR}/request)					
set(REQUEST_SRC		${REQUEST_DIR}/request.cpp
					${REQUEST_DIR}/ac_load.cpp
					${REQUEST_DIR}/custom.cpp
					${REQUEST_DIR}/job.cpp
					${REQUEST_DIR}/packet.cpp
					${REQUEST_DIR}/system.cpp
					${REQUEST_DIR}/net.cpp
					${REQUEST_DIR}/ota.cpp
					${REQUEST_DIR}/app.cpp
					${REQUEST_DIR}/time.cpp
					${REQUEST_DIR}/in_progress.cpp
					)

set(RESOURCE_DIR	${DEVICE_DIR}/resources)
set(RESOURCE_SRC	${RESOURCE_DIR}/route.cpp
					${RESOURCE_DIR}/config.cpp
					${RESOURCE_DIR}/full_config.cpp
					${RESOURCE_DIR}/board_config.cpp
					${RESOURCE_DIR}/sensor_data.cpp
					${RESOURCE_DIR}/time.cpp
					${RESOURCE_DIR}/ota.cpp
					${RESOURCE_DIR}/app.cpp
					${RESOURCE_DIR}/job.cpp
					${RESOURCE_DIR}/info.cpp
					${RESOURCE_DIR}/init.cpp
					${RESOURCE_DIR}/process.cpp)					
					
set(USER_DIR		${SRC_DIR}/user)
set(USER_SRC		${USER_DIR}/user.cpp
					${USER_DIR}/info.cpp
					${USER_DIR}/group.cpp
					${USER_DIR}/list.cpp
					${USER_DIR}/policy.cpp
					${USER_DIR}/password.cpp
					${USER_DIR}/message/auth_message.cpp
					${USER_DIR}/message/user.cpp)

set(DB_DIR			${SRC_DIR}/db)
set(DB_SRC			${DB_DIR}/db.cpp
					${DB_DIR}/user.cpp
					${DB_DIR}/device.cpp
					${DB_DIR}/sensor.cpp
					${DB_DIR}/image.cpp
					${DB_DIR}/app.cpp
					${DB_DIR}/notify.cpp
					${DB_DIR}/report.cpp
					${DB_DIR}/helper.cpp)
					
set(HELPER_DIR		${SRC_DIR}/helper)
set(HELPER_SRC		${HELPER_DIR}/sha256.cpp
					${HELPER_DIR}/time_helper.cpp
					${HELPER_DIR}/coap_helper.cpp)

set(LIBS_SRC		${LIBS_DIR}/sqlite3/sqlite3.cpp)
					
set(INSTANCE_DIR	${SRC_DIR}/instance)
set(INSTANCE_SRC	${INSTANCE_DIR}/agro.cpp
					${INSTANCE_DIR}/check_root.cpp
					${INSTANCE_DIR}/notify.cpp
					${INSTANCE_DIR}/in_progress.cpp
					${INSTANCE_DIR}/user.cpp
					${INSTANCE_DIR}/device.cpp
					${INSTANCE_DIR}/tree.cpp
					${INSTANCE_DIR}/sensor.cpp
					${INSTANCE_DIR}/report.cpp
					${INSTANCE_DIR}/image.cpp
					${INSTANCE_DIR}/app.cpp
					${INSTANCE_DIR}/authenticate.cpp)

set(MAIN_SRC 		${SRC_DIR}/main.cpp 
					${SRC_DIR}/error.cpp
					${SRC_DIR}/arguments.cpp
					${INSTANCE_SRC}
					${MESSAGE_SRC}
					${DEVICE_SRC}
					${SENSOR_SRC}
					${RESOURCE_SRC}
					${REQUEST_SRC}
					${IMAGE_SRC}
					${APP_SRC}
					${NOTIFY_SRC}
					${DB_SRC}
					${USER_SRC}
					${HELPER_SRC}
					${LIBS_SRC})