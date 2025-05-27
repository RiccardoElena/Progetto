package furhatos.app.newskill.data.remote.protocol

sealed class ProtocolMessage(
    val code: UInt,
) {
    override fun toString(): String = "$code|$payload"

    abstract var payload: String

    sealed class RequestType(
        code: UInt,
    ) : ProtocolMessage(code) {
        companion object {
            operator fun invoke(
                code: UInt,
                payload: String,
            ): RequestType =
                when (code) {
                    1u -> AIRequest(payload)
                    2u -> TestRequest(payload)
                    else -> throw IllegalArgumentException("Unsupported message type $code")
                }
        }
    }

    data class AIRequest(
        override var payload: String,
    ) : RequestType(0x01u)

    data class TestRequest(
        override var payload: String,
    ) : RequestType(0x02u)

    sealed class Response(
        code: UInt,
    ) : ProtocolMessage(code) {
        companion object {
            operator fun invoke(
                code: UInt,
                payload: String,
            ): Response =
                when (code) {
                    3u -> TestResponse(payload)
                    4u -> TestResponse(payload)
                    else -> Error(payload)
                }
        }
    }

    data class AIResponse(
        override var payload: String,
    ) : Response(0x03u)

    data class TestResponse(
        override var payload: String,
    ) : Response(0x04u)

    data class Error(
        override var payload: String,
    ) : Response(0x05u)

    companion object {
        operator fun invoke(
            code: UInt,
            payload: String,
        ): ProtocolMessage =
            when (code) {
                1u -> AIRequest(payload)
                2u -> TestRequest(payload)
                3u -> TestResponse(payload)
                4u -> TestResponse(payload)
                else -> Error(payload)
            }
    }
}
