package furhatos.app.newskill.data.remote.dto

import kotlinx.serialization.Serializable

@Serializable
data class MessageHistory(
    val contents: List<ConversationLine>,
)
