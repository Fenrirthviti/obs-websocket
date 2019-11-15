#include "Utils.h"

#include "WSRequestHandler.h"

/**
* Toggle the Replay Buffer on/off.
*
* @api requests
* @name StartStopReplayBuffer
* @category replay buffer
* @since 4.2.0
*/
RpcResponse WSRequestHandler::StartStopReplayBuffer(const RpcRequest& request) {
	if (obs_frontend_replay_buffer_active()) {
		obs_frontend_replay_buffer_stop();
	} else {
		Utils::StartReplayBuffer();
	}
	return request.success();
}

/**
* Start recording into the Replay Buffer.
* Will return an `error` if the Replay Buffer is already active or if the
* "Save Replay Buffer" hotkey is not set in OBS' settings.
* Setting this hotkey is mandatory, even when triggering saves only
* through obs-websocket.
*
* @api requests
* @name StartReplayBuffer
* @category replay buffer
* @since 4.2.0
*/
RpcResponse WSRequestHandler::StartReplayBuffer(const RpcRequest& request) {
	if (!Utils::ReplayBufferEnabled()) {
		return request.failed("replay buffer disabled in settings");
	}

	if (obs_frontend_replay_buffer_active() == true) {
		return request.failed("replay buffer already active");
	}

	Utils::StartReplayBuffer();
	return request.success();
}

/**
* Stop recording into the Replay Buffer.
* Will return an `error` if the Replay Buffer is not active.
*
* @api requests
* @name StopReplayBuffer
* @category replay buffer
* @since 4.2.0
*/
RpcResponse WSRequestHandler::StopReplayBuffer(const RpcRequest& request) {
	if (obs_frontend_replay_buffer_active() == true) {
		obs_frontend_replay_buffer_stop();
		return request.success();
	} else {
		return request.failed("replay buffer not active");
	}
}

/**
* Flush and save the contents of the Replay Buffer to disk. This is
* basically the same as triggering the "Save Replay Buffer" hotkey.
* Will return an `error` if the Replay Buffer is not active.
*
* @api requests
* @name SaveReplayBuffer
* @category replay buffer
* @since 4.2.0
*/
RpcResponse WSRequestHandler::SaveReplayBuffer(const RpcRequest& request) {
	if (!obs_frontend_replay_buffer_active()) {
		return request.failed("replay buffer not active");
	}

	OBSOutputAutoRelease replayOutput = obs_frontend_get_replay_buffer_output();

	calldata_t cd = { 0 };
	proc_handler_t* ph = obs_output_get_proc_handler(replayOutput);
	proc_handler_call(ph, "save", &cd);
	calldata_free(&cd);

	return request.success();
}
