package furhatos.app.newskill.data.remote.dto

import kotlinx.serialization.Serializable

@Serializable
data class ConversationRequest(
    val contents: List<ConversationLine>,
)
