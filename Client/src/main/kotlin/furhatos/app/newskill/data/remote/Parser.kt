package furhatos.app.newskill.data.remote

import furhatos.app.newskill.data.remote.dto.MessageHistory
import furhatos.app.newskill.data.remote.protocol.ProtocolMessage
import furhatos.app.newskill.model.PersonalityDisplacement
import furhatos.util.Language
import kotlinx.serialization.encodeToString
import kotlinx.serialization.json.Json

// Replace '\n' with ' ' and add one at the end
// Use java Locale instead of Furhat Language
object Parser {
    fun toRequestPayload(
        pv: PersonalityDisplacement,
        lang: Language,
        history: MessageHistory,
    ): String {
        val request =
            "Extroversion = ${pv.extroversion}, Agreeableness = ${pv.agreeableness}" +
                ", Conscientiousness = ${pv.conscientiousness}, Emotional stability = ${pv.emotionalStability}, " +
                "Openness to experiences ${pv.opennessToExperiences}|${toText(lang)}|"

        val jsonString = Json.encodeToString(history.contents).removeSurrounding("[", "]")

        return (request + jsonString).replace("\n", " ") + "\n"
    }

    private fun toText(lang: Language): String =
        when (lang.code.lowercase()) {
            "fr-fr" -> "french"
            "es-es" -> "spanish"
            "en-gb" -> "english"
            else -> "italian"
        }

    fun toMessage(response: String): ProtocolMessage.Response =
        response.split("|").let {
            ProtocolMessage.Response(it[0].toUInt(), it[1])
        }
}
