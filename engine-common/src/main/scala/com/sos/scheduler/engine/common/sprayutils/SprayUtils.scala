package com.sos.scheduler.engine.common.sprayutils

import com.sos.scheduler.engine.base.convert.As
import scala.util.control.NonFatal
import shapeless.{::, HNil}
import spray.http.HttpHeaders.Accept
import spray.http.{ContentType, HttpHeader, MediaType, StatusCode}
import spray.httpx.marshalling.ToResponseMarshallable.isMarshallable
import spray.httpx.unmarshalling.{ContentExpected, FromStringOptionDeserializer, MalformedContent}
import spray.routing.Directives._
import spray.routing._

/**
  * @author Joacim Zschimmer
  */
object SprayUtils {
  object implicits {
    implicit class RichOption[A](val delegate: Option[A]) extends AnyVal {
      def applyRoute(f: A ⇒ Route): Route =
        delegate match {
          case Some(a) ⇒ f(a)
          case None ⇒ reject
        }
    }
  }

  def completeWithError(status: StatusCode, message: String) =
    mapRequestContext(_.withContentNegotiationDisabled) {
      complete(isMarshallable(status → message))
    }


  def accept(mediaType: MediaType, mediaTypes: MediaType*): Directive0 =
    accept(mediaTypes.toSet + mediaType)

  def accept(mediaTypes: Iterable[MediaType]): Directive0 =
    mapInnerRoute { route ⇒
      headerValueByType[Accept]() {
        case Accept(requestedMediaTypes) if requestedMediaTypes exists { o ⇒ mediaTypes exists o.matches } ⇒
          route
        case _ ⇒
          reject(UnacceptedResponseContentTypeRejection((mediaTypes map { m ⇒ ContentType(m) }).toList))
      }
    }

  /**
    * Passes x iff argument is Some(x).
    */
  def passSome[A](option: Option[A]): Directive1[A] = {
    import shapeless.::
    new Directive1[A] {
      def happly(inner: (A :: HNil) ⇒ Route) =
        option match {
          case Some(o) ⇒ inner(o :: HNil)
          case None ⇒ reject
        }
    }
  }

  def passRight[R](either: Either[String, R]): Directive1[R] =
    new Directive1[R] {
      def happly(inner: (R :: HNil) ⇒ Route) =
        either match {
          case Right(r) ⇒ inner(r :: HNil)
          case Left(message) ⇒ reject(ValidationRejection(message))
        }
    }

  /**
    * Passes x iff argument is true.
    */
  def passIf(condition: Boolean): Directive0 =
    mapInnerRoute { inner ⇒
      if (condition)
        inner
      else
        reject
    }

  def addHeader(header: HttpHeader): Directive0 =
    mapRequest(o ⇒ o.copy(headers = header :: o.headers))

  /*
  private type ParameterMap = Map[String, String]

  private def eatParameterOption(parameterMap: ParameterMap, key: String) =
    new Directive[ParameterMap :: Option[String] :: HNil] {
      override def happly(inner: (ParameterMap :: Option[String] :: HNil) ⇒ Route) =
        inner((parameterMap - key) :: parameterMap.get(key) :: HNil)
    }

  private def eatParameter(parameterMap: ParameterMap, key: String) =
    new Directive[ParameterMap :: String :: HNil] {
      override def happly(inner: (ParameterMap :: String :: HNil) ⇒ Route) =
        parameterMap.get(key) match {
          case Some(value) ⇒ inner((parameterMap - key) :: value :: HNil)
          case None ⇒ reject
        }
    }

  def removeParameters(keys: Set[String]): Directive0 =
    mapRequest { request ⇒
      request.copy(uri = request.uri.copy(query = removeKeysFromQuery(keys, request.uri.query)))
    }

  private def removeKeysFromQuery(keys: Set[String], query: Uri.Query): Uri.Query = {
    query match {
      case Uri.Query.Empty ⇒ Uri.Query.Empty
      case q @ Uri.Query.Cons(key, value, tail, keep) ⇒
        if (keys contains key)
          removeKeysFromQuery(keys, tail)
        else
          Uri.Query.Cons(key, value, removeKeysFromQuery(keys, tail), keep)
      case q: Uri.Query.Raw ⇒ q
    }
  }

  def noParameters(keys: Set[String]): Directive0 =
    mapInnerRoute { inner ⇒
      requestUri { uri ⇒
        if (uri.query.isEmpty)
          inner
        else
          reject(ValidationRejection(s"Invalid parameters: ${keys mkString ", "}"))
      }
    }
*/

  def emptyParameterMap(parameterMap: Map[String, String]) =
    mapInnerRoute { route ⇒
      if (parameterMap.isEmpty) route
      else reject(ValidationRejection(s"Invalid parameters: ${parameterMap.keys mkString ", "}"))
    }

  implicit def asFromStringOptionDeserializer[A](implicit stringAsA: As[String, A]) =
    new FromStringOptionDeserializer[A] {
      def apply(value: Option[String]) =
        value match {
          case Some(string) ⇒
            try Right(stringAsA(string))
            catch { case NonFatal(t) ⇒
              Left(new MalformedContent(t.toString, Some(t)))
            }
          case None ⇒
            Left(ContentExpected)
        }
    }
}